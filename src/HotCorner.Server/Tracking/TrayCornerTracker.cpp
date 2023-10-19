#include "pch.h"
#include "TrayCornerTracker.h"

namespace winrt::HotCorner::Server::Tracking {
	LRESULT TrayCornerTracker::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept {
		switch (message) {
		case TrackMessage:
			return static_cast<LRESULT>(CornerTracker::Start());

		case UntrackMessage:
			return static_cast<LRESULT>(CornerTracker::Stop());

		default:
			break;
		}

		return TrayIcon::HandleMessage(message, wParam, lParam);
	}

	TrayCornerTracker& TrayCornerTracker::Current() {
		static TrayCornerTracker m_current{};
		return m_current;
	}

	CornerTracker::StartupResult TrayCornerTracker::BeginTracking() const noexcept {
		return static_cast<CornerTracker::StartupResult>(Send(TrackMessage));
	}

	CornerTracker::StopResult TrayCornerTracker::StopTracking() const noexcept {
		return static_cast<CornerTracker::StopResult>(Send(UntrackMessage));
	}
}
