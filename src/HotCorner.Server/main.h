#pragma once
#include "Tray/TrayIcon.h"

namespace winrt::HotCorner::Server::Current {
	HINSTANCE Module() noexcept;
	TrayIcon& Notification() noexcept;
}