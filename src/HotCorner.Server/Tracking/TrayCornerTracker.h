#pragma once
#include "CornerTracker.h"
#include <SettingsManager.h>
#include <Tray/TrayIcon.h>

namespace winrt::HotCorner::Server::Tracking {
	class TrayCornerTracker final : public TrayIcon {
		static constexpr uint32_t TrackMessage = 0x421;
		static constexpr uint32_t UntrackMessage = 0x406;
		static constexpr uint32_t DisplayChangeMessage = WM_DISPLAYCHANGE;

		CornerTracker m_tracker;

	protected:
		LRESULT HandleMessage(
			UINT message,
			WPARAM wParam,
			LPARAM lParam) noexcept override final;

		LRESULT HandleTrayMessage(WPARAM wParam, LPARAM lParam) noexcept override final;

	public:
		TrayCornerTracker(HINSTANCE instance, Settings::SettingsManager& settings) noexcept;
		~TrayCornerTracker() noexcept;

		StartupResult BeginTracking() const noexcept;
		StopResult StopTracking() const noexcept;
	};
}
