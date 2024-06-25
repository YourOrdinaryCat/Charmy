#include <chrono>
#include <debugapi.h>
#include <filesystem>
#include <format>
#include <spdlog/spdlog.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <string_view>

namespace winrt::HotCorner::Logging {
	void Start(std::wstring_view name, const std::filesystem::path& path) {
		using std::chrono::system_clock;
		using file_sink = spdlog::sinks::basic_file_sink_mt;
		using debug_sink = spdlog::sinks::windebug_sink_st;

		const auto logPath = path / std::format(
			L"{}-{}.log", name,
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
	}
}