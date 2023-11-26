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
	}

	LRESULT TrayCornerTracker::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept {
		switch (message) {
		case TrackMessage:
			UpdateIcon(IDI_TRAYICON_ON_DARK, IDI_TRAYICON_ON_LIGHT);
			return static_cast<LRESULT>(CornerTracker::Start(m_window));

		case UntrackMessage:
			UpdateIcon(IDI_TRAYICON_OFF_DARK, IDI_TRAYICON_OFF_LIGHT);
			return static_cast<LRESULT>(CornerTracker::Stop());

		case DisplayChangeMessage:
			OutputDebugString(L"Requesting refresh\n");
			CornerTracker::RequestRefresh();
			break;

		case WM_INPUT: {
			const auto ri = reinterpret_cast<HRAWINPUT>(lParam);

			UINT dataSize{};
			GetRawInputData(
				ri,
				RID_INPUT,
				nullptr,
				&dataSize,
				sizeof(RAWINPUTHEADER)
			);

			if (dataSize > 0) {
				std::vector<BYTE> rawData(dataSize);
				const auto size = GetRawInputData(
					ri,
					RID_INPUT,
					rawData.data(),
					&dataSize,
					sizeof(RAWINPUTHEADER)
				);

				if (size == dataSize) {
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawData.data());
					if (raw->header.dwType == RIM_TYPEMOUSE) {
						CornerTracker::ProcessCurrentPosition();
					}
				}
			}
			return 0;
		}

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
