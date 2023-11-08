#include "pch.h"
#include "TrayCornerTracker.h"
#include "../Resources.h"

namespace winrt::HotCorner::Server::Tracking {
	TrayCornerTracker::TrayCornerTracker() noexcept :
		TrayIcon(Current::Module(), __uuidof(IUnknown))
	{
		const auto tip = Resources::GetString<128>(IDS_TRAY_TOOLTIP);

		UpdateToolTip(tip.data());
		UpdateIcon(IDI_TRAYICONDARK, IDI_TRAYICONLIGHT);
	}

	LRESULT TrayCornerTracker::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept {
		switch (message) {
		case TrackMessage:
			return static_cast<LRESULT>(CornerTracker::Start());

		case UntrackMessage:
			return static_cast<LRESULT>(CornerTracker::Stop());

		case DisplayChangeMessage:
			OutputDebugString(L"Requesting refresh\n");

			CornerTracker::RequestRefresh();
			return 0;

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
