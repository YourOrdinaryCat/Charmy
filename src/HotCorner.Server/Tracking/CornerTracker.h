#pragma once
#include <SettingsManager.h>
#include <Tray/TrayIcon.h>

namespace winrt::HotCorner::Server {
	enum class ActiveCorner {
		TopLeft = 0,
		TopRight = 1,
		BottomLeft = 2,
		BottomRight = 3,
	};

	enum class StartupResult {
		Started = 0,
		AlreadyStarted = 1,
		Failed = 2
	};

	enum class StopResult {
		Stopped = 0,
		NotRunning = 1,
		Failed = 2
	};

	class CornerTracker {
		using ActiveDisplayCorner = std::pair<std::wstring, ActiveCorner>;
		using CornerOffsets = std::array<RECT, 4>;
		using DisplayCorners = std::pair<std::wstring, CornerOffsets>;

		Settings::SettingsManager& m_settings;
		TrayIcon& m_trayIcon;

		HANDLE m_cornerEvent = NULL;
		HANDLE m_waitHandle = INVALID_HANDLE_VALUE;

		bool m_running = false;
		bool m_shouldRefresh = true;

		Settings::CornerAction m_currentAction = Settings::CornerAction::None;
		std::vector<DisplayCorners> m_displayCorners{};

		void TriggerCurrentAction() const noexcept;

		/**
		 * @brief This method runs when the cursor enters the hot corner, after a
		 *        registered wait with a timeout equivalent to the desired delay. If the
		 *        wait times out, the corner function is executed. If the object is
		 *        signaled, the method does nothing.
		*/
		static void NTAPI OnHotCornerTriggered(PVOID action, BOOLEAN timerOrWaitFired) noexcept {
			if (timerOrWaitFired == TRUE) {
				reinterpret_cast<CornerTracker*>(action)->TriggerCurrentAction();
			}
		}

		/**
		 * @brief Enumerates the currently connected displays, and replaces the current
		 *        monitor corner locations.
		*/
		void RefreshDisplays();

		/**
		 * @brief Based on the provided monitor RECT, adds monitor corner locations to
		 *        the hot corner vector.
		*/
		void AddCornerOffsets(std::wstring_view id, const RECT& rect);

		/**
		 * @brief Attempts to pin down the hot corner the provided point belongs to.
		 *
		 * @returns If the point is not within a hot corner, returns nullopt. Otherwise,
		 *          a pair that contains the monitor ID and corner the mouse is in.
		*/
		std::optional<ActiveDisplayCorner> GetActiveCorner(POINT pt);

		/**
		 * @brief Based on the provided display and active corner, begins execution of
		 *        the associated action (if any).
		*/
		void ProcessActiveCorner(std::wstring_view displayId, ActiveCorner corner);

	public:
		constexpr CornerTracker(Settings::SettingsManager& settings, TrayIcon& tracker) noexcept :
			m_settings(settings), m_trayIcon(tracker) { }

		StartupResult Start(HWND window) noexcept;
		StopResult Stop() noexcept;

		void RequestRefresh() noexcept;
		void ProcessCurrentPosition();
	};
}
