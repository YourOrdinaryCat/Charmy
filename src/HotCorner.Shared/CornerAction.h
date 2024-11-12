#pragma once

namespace winrt::HotCorner::Settings {
	enum class CornerAction {
		None = 0,
		TaskView = 1,
		Start = 2,
		Search = 3,
		GoToDesktop = 4,
		QuickSettings = 5,
		PreviousVirtualDesktop = 6,
		NextVirtualDesktop = 7,
	};
}
