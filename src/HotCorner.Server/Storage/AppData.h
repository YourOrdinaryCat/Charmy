#pragma once
#include <filesystem>
#include <optional>

namespace winrt::HotCorner::Server::AppData {
	/**
	 * @brief Whether the app is running within a packaged environment.
	*/
	bool IsPackaged() noexcept;

	/**
	 * @brief Gets the path to the app's Roaming folder.
	*/
	std::filesystem::path Roaming();
}