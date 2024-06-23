#include "pch.h"
#include "server.h"
#include "App.h"
#include "LifetimeManager.h"
#include "Storage/AppData.h"
#include "Tracking/TrayCornerTracker.h"
#include <format>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

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

		using std::chrono::system_clock;
		using file_sink = spdlog::sinks::basic_file_sink_mt;
		using debug_sink = spdlog::sinks::windebug_sink_st;

		const auto settings = AppData::Roaming();
		const auto logPath = settings / std::format(
			L"Server-{}.log",
			system_clock::to_time_t(system_clock::now())
		);

		auto fileSink = std::make_shared<file_sink>(logPath.string());
		fileSink->set_level(spdlog::level::warn);

		auto fileLog = std::make_shared<spdlog::logger>("", fileSink);
		if (IsDebuggerPresent()) {
			auto debugSink = std::make_shared<debug_sink>();
			debugSink->set_level(spdlog::level::trace);

			fileLog->sinks().push_back(debugSink);
		}

		spdlog::set_formatter(std::make_unique<spdlog::pattern_formatter>(
			spdlog::pattern_time_type::utc
		));
		spdlog::set_level(spdlog::level::trace);
		spdlog::flush_on(spdlog::level::trace);
		spdlog::initialize_logger(fileLog);
		spdlog::set_default_logger(fileLog);

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
