#include "pch.h"

#include "App.h"
#include "LifetimeManager.h"
#include "Logging.h"
#include "server.h"
#include "Storage/AppData.h"
#include "Tracking/TrayCornerTracker.h"
#include <cstdlib>

namespace winrt::HotCorner::Server {
	namespace impl = implementation;

	static constexpr std::wstring_view InstanceMutexName
		= L"HotCorner_w2v1h8dyp9x88!ServerMutex";

	static void OnExit() {
		spdlog::debug("Standard shutdown started");
		spdlog::shutdown();
	}

	extern "C" int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR pCmdLine, int) {
		CreateMutex(NULL, TRUE, InstanceMutexName.data());
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			return 1;
		}

		winrt::init_apartment(apartment_type::multi_threaded);

		// We don't support unpackaged execution at the moment - show a short message
		// explaining that, in case a developer accidentally sets this project as the
		// startup one
		if (!AppData::IsPackaged()) {
#if defined _DEBUG
			MessageBoxA(NULL, "Set HotCorner.Package as the startup project.", "Unable to launch Charmy", MB_ICONERROR);
#endif
			return 0;
		}

		const auto settings = AppData::Roaming();
		Logging::Start(L"Server", settings);
		std::atexit(OnExit);

		App app{ instance, settings };
		app.LoadSettings();

		Logging::FileSink()->set_level(app.Settings().LogVerbosity);

		const DWORD cookie = server::register_class<impl::LifetimeManager, App&>(app);
		server::resume_class_objects();

		// If auto startup is enabled, we won't get this argument, which means
		// we have to initialize tracking right away
		if (wcscmp(pCmdLine, L"-srv") != 0) {
			if (app.Settings().TrackingEnabled) {
				app.TrayIcon().BeginTracking();
			}

			if (app.Settings().TrayIconEnabled) {
				ShowTrayIcon(app.TrayIcon());
			}
		}

		const int result = app.Run();
		server::unregister_class(cookie);

		return result;
	}
}
