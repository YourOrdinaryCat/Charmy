#pragma once
#include <CornerAction.h>
#include <Tray/TrayIcon.h>

namespace winrt::HotCorner::Server::Tracking {
	bool RunAction(const TrayIcon& icon, Settings::CornerAction action) noexcept;
}
