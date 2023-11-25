#include "pch.h"
#include "TrayCornerTracker.h"
#include "../Resources.h"

namespace winrt::HotCorner::Server::Tracking {
	TrayCornerTracker::TrayCornerTracker() noexcept :
		TrayIcon(Current::Module(), __uuidof(IUnknown))
	{
		const auto tip = Resources::GetString<128>(IDS_TRAY_TOOLTIP);
		UpdateToolTip(tip.data());

		SetHighContrastIcon(IDI_TRAYICON_HC_DARK, IDI_TRAYICON_HC_LIGHT);
		UpdateIcon(IDI_TRAYICON_OFF_DARK, IDI_TRAYICON_OFF_LIGHT);
	}

	LRESULT TrayCornerTracker::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept {
		switch (message) {
		case TrackMessage:
			UpdateIcon(IDI_TRAYICON_ON_DARK, IDI_TRAYICON_ON_LIGHT);
			return static_cast<LRESULT>(CornerTracker::Start());

		case UntrackMessage:
			UpdateIcon(IDI_TRAYICON_OFF_DARK, IDI_TRAYICON_OFF_LIGHT);
			return static_cast<LRESULT>(CornerTracker::Stop());

		case DisplayChangeMessage:
			OutputDebugString(L"Requesting refresh\n");
			CornerTracker::RequestRefresh();
			break;

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
