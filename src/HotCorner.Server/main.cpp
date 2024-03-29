#include "pch.h"
#include "main.h"
#include "server.h"

#include "LifetimeManager.h"
#include "Storage/AppData.h"
#include "Tracking/TrayCornerTracker.h"

namespace winrt::HotCorner::Server::Current {
	static HINSTANCE m_instance = nullptr;
	HINSTANCE Module() noexcept {
		return m_instance;
	}

	static std::filesystem::path GetSettingsPath() {
		if (const auto path = AppData::Roaming()) {
			return *path;
		}
		throw_hresult(APPMODEL_ERROR_NO_PACKAGE);
	}

	static Settings::SettingsManager m_settings{ GetSettingsPath() };
	Settings::SettingsManager& Settings() noexcept {
		return m_settings;
	}

	extern "C" int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR pCmdLine, int) {
		winrt::init_apartment(apartment_type::multi_threaded);
		if (!m_instance) {
			m_instance = instance;
			m_settings.Load();
		}

		DWORD cookie{};
		server::register_class<implementation::LifetimeManager>(&cookie);

		CoResumeClassObjects();

		// If auto startup is enabled, we won't get this argument, which means
		// we have to initialize tracking right away
		if (wcscmp(pCmdLine, L"-Embedding") != 0) {
			if (m_settings.TrackingEnabled) {
				TrackHotCorners();
			}

			if (m_settings.TrayIconEnabled) {
				ShowTrayIcon();
			}
		}

		const auto result = Tracking::TrayCornerTracker::Current().RunMessageLoop();
		server::unregister_class(cookie);

		return static_cast<int>(result);
	}
}
