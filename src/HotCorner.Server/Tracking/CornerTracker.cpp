#include "pch.h"
#include "CornerTracker.h"
#include "CornerActions.h"
#include "hidusage.h"
#include "../main.h"

namespace winrt::HotCorner::Server::CornerTracker {
	using CornerOffsets = std::array<RECT, 4>;
	using DisplayCorners = std::pair<std::wstring, CornerOffsets>;
	using ActiveDisplayCorner = std::pair<std::wstring, ActiveCorner>;

	std::thread m_actionThread{};
	HANDLE m_cornerEvent = INVALID_HANDLE_VALUE;

	bool m_running = false;
	bool m_shouldRefresh = true;

	std::vector<DisplayCorners> m_displayCorners{};
	constexpr LONG m_offset = 10;

	static bool IsPointWithinRect(const RECT& rect, POINT pt) noexcept {
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
	static std::optional<ActiveDisplayCorner> GetActiveHotCorner(const POINT& pt) {
		for (const auto& dc : m_displayCorners) {
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

		DISPLAY_DEVICE display{};
		ZeroMemory(&display, sizeof(display));
		display.cb = sizeof(display);

		DEVMODE dm{};

		DWORD index = 0;
		while (EnumDisplayDevices(NULL, index, &display, 0)) {
			std::array<WCHAR, 32> name{};
			wcscpy_s(name.data(), name.size(), display.DeviceName);

			// Only add displays that are part of the desktop
			if (display.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {
				if (!EnumDisplayDevices(name.data(), 0, &display, EDD_GET_DEVICE_INTERFACE_NAME)) {
					//TODO: Handle failure
					OutputDebugString(L"Failed to get display ID\n");
					goto next_display;
				}

				ZeroMemory(&dm, sizeof(DEVMODE));
				dm.dmSize = sizeof(DEVMODE);

				if (!EnumDisplaySettings(name.data(), ENUM_CURRENT_SETTINGS, &dm)) {
					//TODO: Handle failure
					OutputDebugString(L"Failed to get screen rect\n");
					goto next_display;
				}

				const RECT screenRect{
					.left = dm.dmPosition.x,
					.top = dm.dmPosition.y,
					.right = dm.dmPosition.x + static_cast<LONG>(dm.dmPelsWidth),
					.bottom = dm.dmPosition.y + static_cast<LONG>(dm.dmPelsHeight)
				};

				AddCornerOffsets(display.DeviceID, screenRect);
			}

		next_display:
			ZeroMemory(&display, sizeof(display));
			display.cb = sizeof(display);
			++index;
		}
	}

	/**
	 * @brief This thread runs when the cursor enters the hot corner, and waits for
	 *        its reset event with a timeout equivalent to the desired delay. If the
	 *        wait times out, the corner function is executed. If the object is signaled,
	 *        the thread exits.
	*/
	static void OnHotCornerEntry(std::pair<std::function<bool()>, uint32_t> param) noexcept {
		const auto result = WaitForSingleObject(m_cornerEvent, param.second);
		if (result == WAIT_TIMEOUT) {
			if (!param.first()) {
				//TODO: Handle failure
				OutputDebugString(L"Failed to perform user-defined action\n");
			}
		}
		else if (result == WAIT_FAILED) {
			//TODO: Handle failure
			OutputDebugString(L"Failed to wait for hot corner event\n");
		}
	}

	static std::optional<std::function<bool()>> GetAction(const Settings::MonitorSettings& settings, ActiveCorner corner) noexcept {
		switch (corner) {
		case ActiveCorner::TopLeft:
			return Tracking::GetDelegate(settings.TopLeftAction);
		case ActiveCorner::TopRight:
			return Tracking::GetDelegate(settings.TopRightAction);
		case ActiveCorner::BottomLeft:
			return Tracking::GetDelegate(settings.BottomLeftAction);
		case ActiveCorner::BottomRight:
			return Tracking::GetDelegate(settings.BottomRightAction);
		}

		return std::nullopt;
	}

	static uint32_t GetDelay(const Settings::MonitorSettings& settings, ActiveCorner corner) noexcept {
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

		return std::numeric_limits<uint32_t>::max();
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
		if (m_shouldRefresh && !m_actionThread.joinable()) {
			m_shouldRefresh = false;
			OutputDebugString(L"Refreshing displays\n");
			RefreshDisplays();
		}

		static POINT pt{};
		if (GetCursorPos(&pt) == FALSE) {
			return;
		}

		if (const auto corner = GetActiveHotCorner(pt)) {
			// The corner is hot, check if it was already hot
			if (m_actionThread.joinable()) {
				return;
			}

			// The corner is hot, and was previously cold
			// If there's an associated action, start a tracking thread
			static const auto& settings = Current::Settings();

			const auto setting = settings.GetSettingOrDefaults(corner->first);
			const auto active = corner->second;

			if (const auto action = GetAction(setting, active)) {
				m_cornerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				m_actionThread = std::thread(
					OnHotCornerEntry,
					std::pair{ *action, GetDelay(setting, active) }
				);
			}
		}
		else {
			// The corner is cold, and was cold before
			if (!m_actionThread.joinable()) {
				return;
			}

			// The corner is cold, but was previously hot
			SetEvent(m_cornerEvent);
			CloseHandle(m_cornerEvent);

			// We have to reset state here
			m_actionThread.join();

			m_actionThread = {};
			m_cornerEvent = INVALID_HANDLE_VALUE;
		}
	}
}
