#pragma once
#include <CornerAction.h>

namespace winrt::HotCorner::Server::Tracking {
	bool RunAction(const Settings::CornerAction action) noexcept;
}
