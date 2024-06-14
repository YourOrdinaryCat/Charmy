#include "pch.h"
#include "App.h"
#include "LifetimeManager.h"
#include "Storage/AppData.h"
#include "Tracking/TrayCornerTracker.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/msvc_sink.h"
#include "server.h"

namespace winrt::HotCorner::Server {
	namespace impl = implementation;

	static std::filesystem::path GetRoamingPath() {
		if (const auto path = AppData::Roaming()) {
			return *path;
		}
		throw_hresult(APPMODEL_ERROR_NO_PACKAGE);
	}

	extern "C" int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR pCmdLine, int) {
		winrt::init_apartment(apartment_type::multi_threaded);

		const auto logger = std::make_shared<spdlog::logger>(
			"", std::make_shared<spdlog::sinks::windebug_sink_st>()
		);

		spdlog::set_formatter(std::make_unique<spdlog::pattern_formatter>(
			spdlog::pattern_time_type::utc
		));
		spdlog::set_level(spdlog::level::trace);
		spdlog::initialize_logger(logger);
		spdlog::set_default_logger(logger);

		const auto settings = GetRoamingPath();

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

		return result;
	}
}
