#pragma once

namespace winrt::HotCorner::Server {
	/**
	 * @brief Returns whether the user is on Windows 10 1903, or a future version.
	*/
	bool IsAtLeast1903() noexcept;
}
