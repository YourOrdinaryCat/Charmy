#include "pch.h"
#include "main.h"

#include "LifetimeManager.h"
#include "Tracking/TrayCornerTracker.h"
#include <winrt/Windows.Storage.h>

import server;

namespace winrt::HotCorner::Server::Current {
	static HINSTANCE m_instance = nullptr;

	HINSTANCE Module() noexcept {
		return m_instance;
	}

	Settings::SettingsManager& Settings() noexcept {
		static Settings::SettingsManager m_settings{
			Windows::Storage::ApplicationData::Current().LocalFolder().Path().c_str()
		};
		return m_settings;
	}

	extern "C" int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR pCmdLine, int) {
		if (!m_instance) {
			m_instance = instance;
			Settings().Load();
		}

		winrt::init_apartment();

		const auto cookies = server::register_classes<implementation::LifetimeManager>();
		CoResumeClassObjects();

		// If auto startup is enabled, we won't get this argument, which means
		// we have to initialize tracking right away
		if (wcscmp(pCmdLine, L"-Embedding") != 0) {
			if (Settings().TrackingEnabled) {
				TrackHotCorners();
			}

			if (Settings().TrayIconEnabled) {
				ShowTrayIcon();
			}
		}

		const auto result = Tracking::TrayCornerTracker::Current().RunMessageLoop();
		server::unregister_classes(cookies);

		return static_cast<int>(result);
	}
}
