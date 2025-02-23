#include "pch.h"
#include "TrayCornerTracker.h"
#include <appmodel.h>
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

	LRESULT TrayCornerTracker::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) noexcept {
		switch (message) {
		case TrackMessage:
			UpdateIcon(IDI_TRAYICON_ON_DARK, IDI_TRAYICON_ON_LIGHT);
			return HandleAsyncMessage(
				lParam, static_cast<LRESULT>(m_tracker.Start(m_window)));

		case UntrackMessage:
			UpdateIcon(IDI_TRAYICON_OFF_DARK, IDI_TRAYICON_OFF_LIGHT);
			return HandleAsyncMessage(
				lParam, static_cast<LRESULT>(m_tracker.Stop()));

		case DisplayChangeMessage:
			spdlog::debug("Requesting refresh");
			m_tracker.RequestRefresh();
			return 0;

		case WM_INPUT: {
			constexpr UINT riSize = sizeof(RAWINPUT);
			constexpr UINT rihSize = sizeof(RAWINPUTHEADER);

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

	static std::wstring GetAumid() {
		UINT32 length = 0;
		LONG ret = GetCurrentApplicationUserModelId(&length, NULL);

		if (ret == ERROR_INSUFFICIENT_BUFFER) {
			std::wstring aumid(length - 1, '\0');
			ret = GetCurrentApplicationUserModelId(&length, aumid.data());

			if (ret == ERROR_SUCCESS) {
				return aumid;
			}
		}

		throw_hresult(ret);
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

			const auto aumid = GetAumid();

			DWORD pid;
			const auto hr = am->ActivateApplication(aumid.data(), L"", AO_NONE, &pid);

			if (hr != S_OK) {
				spdlog::error("Unable to launch Charmy. HRESULT: {}", hr);
			}
			return 0;
		}
		return TrayIcon::HandleTrayMessage(wParam, lParam);
	}

	void TrayCornerTracker::HandleTrackingStartResult(StartupResult result) noexcept {
		if (result == StartupResult::Started) {
			CoAddRefServerProcess();
		}
	}

	void TrayCornerTracker::HandleTrackingStopResult(StopResult result) noexcept {
		if (result == StopResult::Stopped && CoReleaseServerProcess() == 0) {
			Close();
		}
	}

	StartupResult TrayCornerTracker::BeginTracking() noexcept {
		const auto result = static_cast<StartupResult>(Send(TrackMessage));
		HandleTrackingStartResult(result);
		return result;
	}

	StopResult TrayCornerTracker::StopTracking() noexcept {
		const auto result = static_cast<StopResult>(Send(UntrackMessage));
		HandleTrackingStopResult(result);
		return result;
	}

	async::task<StartupResult> TrayCornerTracker::BeginTrackingAsync() {
		const auto result = co_await SendAsync(TrackMessage);
		if (result.has_value()) {
			const auto sr = static_cast<StartupResult>(*result);
			HandleTrackingStartResult(sr);
			co_return sr;
		}
		co_return StartupResult::Failed;
	}

	async::task<StopResult> TrayCornerTracker::StopTrackingAsync() {
		const auto result = co_await SendAsync(UntrackMessage);
		if (result.has_value()) {
			const auto sr = static_cast<StopResult>(*result);
			HandleTrackingStopResult(sr);
			co_return sr;
		}
		co_return StopResult::Failed;
	}
}
