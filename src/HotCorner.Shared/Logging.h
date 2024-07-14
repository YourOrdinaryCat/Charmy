#pragma once
#include <chrono>
#include <errhandlingapi.h>
#include <filesystem>
#include <lazy_file_sink.h>
#include <spdlog/spdlog.h>
#include <string_view>

#define SPDLOG_LAST_ERROR(LEVEL, MESSAGE) \
const auto lastErrorCode = GetLastError(); \
spdlog::log(LEVEL, MESSAGE ". Error: {}", lastErrorCode); \
if constexpr (LEVEL == spdlog::level::critical) { \
	__fastfail(FAST_FAIL_FATAL_APP_EXIT); \
}

namespace winrt::HotCorner::Logging {
	std::shared_ptr<lazy_file_sink_mt> FileSink() noexcept;
	void Start(std::wstring_view name, const std::filesystem::path& path);
}