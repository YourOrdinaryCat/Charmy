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

	TrayCornerTracker::~TrayCornerTracker() noexcept {
		StopTracking();
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
			static constexpr UINT riSize = sizeof(RAWINPUT);
			static constexpr UINT rihSize = sizeof(RAWINPUTHEADER);

			static auto pcbSize = riSize;
			static std::array<BYTE, riSize> lpb{};

			GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				lpb.data(),
				&pcbSize,
				rihSize
			);

			const auto raw = reinterpret_cast<RAWINPUT*>(lpb.data());

			if (raw->header.dwType == RIM_TYPEMOUSE) {
				if (raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0) {
					CornerTracker::ProcessCurrentPosition();
					return 0;
				}
			}
			return DefRawInputProc(NULL, NULL, rihSize);
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
