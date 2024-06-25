#include "pch.h"
#include "server.h"
#include "App.h"
#include "LifetimeManager.h"
#include "Logging.h"
#include "Storage/AppData.h"
#include "Tracking/TrayCornerTracker.h"

namespace winrt::HotCorner::Server {
	namespace impl = implementation;

	extern "C" int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
		winrt::init_apartment(apartment_type::multi_threaded);

		// We don't support unpackaged execution at the moment - show a short message
		// explaining that, as a treat
		if (!AppData::IsPackaged()) {
#if defined _DEBUG
			MessageBoxA(NULL, "Set HotCorner.Package as the startup project.", "Unable to launch Charmy", MB_ICONERROR);
#else
			MessageBoxA(NULL, "If you tried to open the .exe directly, this is expected. Always launch Charmy from the start menu, and manage auto startup through the Settings app or Task Manager. If this shows up unexpectedly, let the developer know.", "Unable to launch Charmy", MB_ICONERROR);
#endif
			return 0;
		}

		const auto settings = AppData::Roaming();
		Logging::Start(L"Server", settings);

		App app{ instance, settings };
		app.Settings().Load();

		const auto cookie = server::register_class<impl::LifetimeManager, App&>(app);
		server::resume_class_objects();

		if (app.Settings().TrackingEnabled) {
			TrackHotCorners(app.TrayIcon());
		}

		if (app.Settings().TrayIconEnabled) {
			ShowTrayIcon(app.TrayIcon());
		}

		const auto result = app.Run();

		server::unregister_class(cookie);
		spdlog::shutdown();

		return result;
	}
}
