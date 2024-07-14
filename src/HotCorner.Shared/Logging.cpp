#include "pch.h"
#include "Logging.h"
#include <chrono>
#include <debugapi.h>
#include <format>
#include <spdlog/sinks/msvc_sink.h>

namespace winrt::HotCorner::Logging {
	static std::weak_ptr<lazy_file_sink_mt> m_fileSink{};

	std::shared_ptr<lazy_file_sink_mt> FileSink() noexcept {
		if (auto sink = m_fileSink.lock()) {
			return sink;
		}
		return nullptr;
	}

	static void OnLoggerError(const std::string& message) {
		if (IsDebuggerPresent()) {
			static std::atomic_size_t counter = 0;
			OutputDebugStringA(std::format("[*** LOG ERROR {:04} ***] {}\n", counter++, message).c_str());
		}
	}

	void Start(std::wstring_view name, const std::filesystem::path& path) {
		using std::chrono::system_clock;
		using file_sink = lazy_file_sink_mt;
		using debug_sink = spdlog::sinks::windebug_sink_st;

		const auto logPath = path / std::format(
			L"{}-{}.log", name,
			system_clock::to_time_t(system_clock::now())
		);

		auto fileSink = std::make_shared<file_sink>(logPath);
		fileSink->set_level(spdlog::level::warn);

		m_fileSink = fileSink;
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
		spdlog::set_error_handler(OnLoggerError);
		spdlog::initialize_logger(fileLog);
		spdlog::set_default_logger(fileLog);
	}
}