#pragma once
#include <filesystem>
#include <optional>

namespace winrt::HotCorner::Server::AppData {
	/**
	 * @brief Gets the path to the app's Roaming folder. This method expects a packaged
	 *        environment - if the expectation fails, nullopt is returned.
	*/
	std::optional<std::filesystem::path> Roaming();
}