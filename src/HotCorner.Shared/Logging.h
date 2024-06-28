#include <chrono>
#include <errhandlingapi.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <string_view>

#define SPDLOG_LAST_ERROR(LEVEL, MESSAGE) \
const auto lastErrorCode = GetLastError(); \
spdlog::log(LEVEL, MESSAGE ". Error: {}", lastErrorCode)

namespace winrt::HotCorner::Logging {
	void Start(std::wstring_view name, const std::filesystem::path& path);
}