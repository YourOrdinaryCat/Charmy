#include "pch.h"
#include "CornerTracker.h"
#include "CornerActions.h"

#include <array>
#include <Devices/Display.h>
#include <hidusage.h>
#include <main.h>
#include <Unknwn.h>

namespace winrt::HotCorner::Server::CornerTracker {
	using ActionT = Settings::CornerAction;
	using SettingsT = Settings::MonitorSettings;

	using CornerOffsets = std::array<RECT, 4>;
	using DisplayCorners = std::pair<std::wstring, CornerOffsets>;
	using ActiveDisplayCorner = std::pair<std::wstring, ActiveCorner>;

	HANDLE m_cornerEvent = NULL;
	HANDLE m_waitHandle = INVALID_HANDLE_VALUE;

	bool m_running = false;
	bool m_shouldRefresh = true;

	std::vector<DisplayCorners> m_displayCorners{};
	constexpr LONG m_offset = 10;

	static constexpr bool IsPointWithinRect(const RECT& rect, POINT pt) noexcept {
		if (pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top) {
			return pt.y < rect.bottom;
		}
		return false;
	}

	/**
	 * @brief Attempts to pin down the hot corner the provided point belongs to.
	 *
	 * @returns If the point is not within a hot corner, returns nullopt. Otherwise,
	 *          a pair that contains the monitor ID and corner the mouse is in.
	*/
	static std::optional<ActiveDisplayCorner> GetActiveCorner(POINT pt) {
		for (auto&& dc : m_displayCorners) {
			for (uint32_t i = 0; i < dc.second.size(); ++i) {
				if (IsPointWithinRect(dc.second[i], pt)) {
					return { { dc.first, static_cast<ActiveCorner>(i) } };
				}
			}
		}
		return std::nullopt;
	}

	/**
	 * @brief Based on the provided monitor RECT, adds monitor corner locations to
	 *        the hot corner vector.
	*/
	static void AddCornerOffsets(std::wstring_view id, const RECT& rect) {
		const RECT tlo{
			.left = rect.left - m_offset,
			.top = rect.top - m_offset,
			.right = rect.left + m_offset,
			.bottom = rect.top + m_offset,
		};

		const RECT tro{
			.left = rect.right - m_offset,
			.top = rect.top - m_offset,
			.right = rect.right + m_offset,
			.bottom = rect.top + m_offset,
		};

		const RECT blo{
			.left = rect.left - m_offset,
			.top = rect.bottom - m_offset,
			.right = rect.left + m_offset,
			.bottom = rect.bottom + m_offset,
		};

		const RECT bro{
			.left = rect.right - m_offset,
			.top = rect.bottom - m_offset,
			.right = rect.right + m_offset,
			.bottom = rect.bottom + m_offset,
		};

		m_displayCorners.emplace_back(id, CornerOffsets{ tlo, tro, blo, bro });
	}

	/**
	 * @brief Enumerates the currently connected displays, and replaces the current
	 *        monitor corner locations.
	*/
	static void RefreshDisplays() {
		m_displayCorners.clear();

		// Enumerate displays that are part of the desktop
		for (auto&& display : Devices::Displays(Devices::DisplayAttachedToDesktop)) {
			const auto name = Devices::GetDisplayName(display);

			if (Devices::TrySetDisplayId(name.data(), &display)) {
				DEVMODE dm{};

				if (Devices::TryGetDisplaySettings(name.data(), &dm)) {
					const RECT screenRect{
						.left = dm.dmPosition.x,
						.top = dm.dmPosition.y,
						.right = dm.dmPosition.x + static_cast<LONG>(dm.dmPelsWidth),
						.bottom = dm.dmPosition.y + static_cast<LONG>(dm.dmPelsHeight)
					};

					AddCornerOffsets(display.DeviceID, screenRect);
				}
				else {
					//TODO: Handle failure
					OutputDebugString(L"Failed to get display settings\n");
				}
			}
			else {
				//TODO: Handle failure
				OutputDebugString(L"Failed to get display ID\n");
			}
		}
	}

	/**
	 * @brief This method runs when the cursor enters the hot corner, after a registered
	 *        wait with a timeout equivalent to the desired delay. If the wait times
	 *        out, the corner function is executed. If the object is signaled, the
	 *        method does nothing.
	*/
	static void NTAPI OnHotCornerEntry(PVOID action, BOOLEAN timerOrWaitFired) noexcept {
		if (timerOrWaitFired == TRUE) {
			// Nasty, but the callback signature only takes void*
			const auto act = static_cast<ActionT>(reinterpret_cast<intptr_t>(action));

			if (!Tracking::RunAction(act)) {
				//TODO: Handle failure
				OutputDebugString(L"Failed to perform user-defined action\n");
			}
		}
	}

	static constexpr ActionT GetAction(const SettingsT& settings, ActiveCorner corner) noexcept {
		switch (corner) {
		case ActiveCorner::TopLeft:
			return settings.TopLeftAction;
		case ActiveCorner::TopRight:
			return settings.TopRightAction;
		case ActiveCorner::BottomLeft:
			return settings.BottomLeftAction;
		case ActiveCorner::BottomRight:
			return settings.BottomRightAction;
		}
		return ActionT::None;
	}

	static constexpr uint32_t GetDelay(const SettingsT& settings, ActiveCorner corner) noexcept {
		if (!settings.DelayEnabled) {
			return 0;
		}

		switch (corner) {
		case ActiveCorner::TopLeft:
			return settings.TopLeftDelay;
		case ActiveCorner::TopRight:
			return settings.TopRightDelay;
		case ActiveCorner::BottomLeft:
			return settings.BottomLeftDelay;
		case ActiveCorner::BottomRight:
			return settings.BottomRightDelay;
		}

		return UINT32_MAX;
	}

	/**
	 * @brief Based on the provided display and active corner, begins execution of the
	 *        associated action (if any).
	*/
	static void ProcessActiveCorner(std::wstring_view displayId, ActiveCorner corner) {
		const auto& setting = Current::Settings().GetSettingOrDefaults(displayId);
		const auto action = GetAction(setting, corner);

		if (action != ActionT::None) {
			SetLastError(0);
			m_cornerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

			if (m_cornerEvent != NULL) {
				SetLastError(0);
				const BOOL registered = RegisterWaitForSingleObject(
					&m_waitHandle,
					m_cornerEvent,
					OnHotCornerEntry,
					// Very nasty cast, but the callback only accepts void*
					reinterpret_cast<void*>(static_cast<intptr_t>(action)),
					GetDelay(setting, corner),
					WT_EXECUTEONLYONCE
				);

				if (registered == FALSE) {
					//TODO: properly log failure
					OutputDebugString(L"Unable to register wait\n");
				}
			}
			else {
				//TODO: properly log failure
				OutputDebugString(L"Unable to create event\n");
			}
		}
	}

	StartupResult Start(HWND window) noexcept {
		if (m_running) {
			return StartupResult::AlreadyStarted;
		}

		RAWINPUTDEVICE rid{
			.usUsagePage = HID_USAGE_PAGE_GENERIC,
			.usUsage = HID_USAGE_GENERIC_MOUSE,
			.dwFlags = RIDEV_INPUTSINK,
			.hwndTarget = window
		};

		m_running = RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
		if (m_running) {
			return StartupResult::Started;
		}

		return StartupResult::Failed;
	}

	StopResult Stop() noexcept {
		if (!m_running) {
			return StopResult::NotRunning;
		}

		RAWINPUTDEVICE rid{
			.usUsagePage = HID_USAGE_PAGE_GENERIC,
			.usUsage = HID_USAGE_GENERIC_MOUSE,
			.dwFlags = RIDEV_REMOVE,
			.hwndTarget = nullptr
		};

		if (RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
			m_running = false;
			return StopResult::Stopped;
		}

		return StopResult::Failed;
	}

	void RequestRefresh() noexcept {
		m_shouldRefresh = true;
	}

	void ProcessCurrentPosition() {
		// If no hot corner is active, and refreshing has been requested,
		// update monitors positions
		if (m_shouldRefresh && m_cornerEvent == NULL) {
			m_shouldRefresh = false;
			OutputDebugString(L"Refreshing displays\n");
			RefreshDisplays();
		}

		POINT pt;
		SetLastError(0);

		if (GetCursorPos(&pt) != FALSE) {
			if (const auto corner = GetActiveCorner(pt)) {
				if (m_cornerEvent == NULL) {
					// The corner is hot, and was previously cold
					ProcessActiveCorner(corner->first, corner->second);
				}
			}
			else if (m_cornerEvent != NULL) {
				// The corner is cold, but was previously hot
				//TODO: failure here is critical, we likely want to abort here
				SetEvent(m_cornerEvent);
				CloseHandle(m_cornerEvent);
				(void)UnregisterWait(m_waitHandle);

				m_cornerEvent = NULL;
				m_waitHandle = INVALID_HANDLE_VALUE;
			}
		}
		else {
			//TODO: properly log failure
			OutputDebugString(L"Unable to create event\n");
		}
	}
}
