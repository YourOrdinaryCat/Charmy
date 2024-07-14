#include "pch.h"
#include "TrayCornerTracker.h"
#include <Resources.h>
#include <ShlObj.h>
#include <spdlog/spdlog.h>

namespace winrt::HotCorner::Server::Tracking {
	static constexpr winrt::guid IconId{ "DFD7D29C-DA63-4B0A-B396-34F656DC7CC2" };

	TrayCornerTracker::TrayCornerTracker(HINSTANCE instance, Settings::SettingsManager& settings) noexcept :
		TrayIcon(instance, IconId), m_tracker(settings, *this)
	{
		UpdateToolTip(L"Charmy");
		SetHighContrastIcon(IDI_TRAYICON_HC_DARK, IDI_TRAYICON_HC_LIGHT, false);
	}

	TrayCornerTracker::~TrayCornerTracker() noexcept {
		StopTracking();
	}

	LRESULT TrayCornerTracker::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept {
		switch (message) {
		case TrackMessage:
			UpdateIcon(IDI_TRAYICON_ON_DARK, IDI_TRAYICON_ON_LIGHT);
			return static_cast<LRESULT>(m_tracker.Start(m_window));

		case UntrackMessage:
			UpdateIcon(IDI_TRAYICON_OFF_DARK, IDI_TRAYICON_OFF_LIGHT);
			return static_cast<LRESULT>(m_tracker.Stop());

		case DisplayChangeMessage:
			spdlog::debug("Requesting refresh");
			m_tracker.RequestRefresh();
			return 0;

		case WM_INPUT: {
			static constexpr UINT riSize = sizeof(RAWINPUT);
			static constexpr UINT rihSize = sizeof(RAWINPUTHEADER);

			UINT pcbSize = riSize;
			std::array<BYTE, riSize> lpb{};

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
					m_tracker.ProcessCurrentPosition();
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

	LRESULT TrayCornerTracker::HandleTrayMessage(WPARAM wParam, LPARAM lParam) noexcept {
		switch (LOWORD(lParam)) {
		case WM_CONTEXTMENU:
		case NIN_KEYSELECT:
		case NIN_SELECT:
			const auto am{
				create_instance<IApplicationActivationManager>(
					CLSID_ApplicationActivationManager,
					CLSCTX_LOCAL_SERVER
				)
			};

			DWORD pid;
			const auto hr = am->ActivateApplication(L"HotCorner_w2v1h8dyp9x88!App", L"", AO_NONE, &pid);

			if (hr != S_OK) {
				spdlog::error("Unable to launch Charmy. HRESULT: {}", hr);
			}
			return 0;
		}
		return TrayIcon::HandleTrayMessage(wParam, lParam);
	}

	StartupResult TrayCornerTracker::BeginTracking() const noexcept {
		return static_cast<StartupResult>(Send(TrackMessage));
	}

	StopResult TrayCornerTracker::StopTracking() const noexcept {
		return static_cast<StopResult>(Send(UntrackMessage));
	}
}
