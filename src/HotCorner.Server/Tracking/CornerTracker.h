#pragma once

namespace winrt::HotCorner::Server::CornerTracker {
	enum class ActiveCorner {
		TopLeft = 0,
		TopRight = 1,
		BottomLeft = 2,
		BottomRight = 3,
	};

	enum class StartupResult {
		Started = 0,
		AlreadyStarted = 1,
		Failed = 2
	};

	enum class StopResult {
		Stopped = 0,
		NotRunning = 1,
		Failed = 2
	};

	StartupResult Start() noexcept;
	StopResult Stop() noexcept;

	void RequestRefresh() noexcept;
}
