#pragma once
#include "CornerTracker.h"
#include "../main.h"
#include "../Tray/TrayIcon.h"

namespace winrt::HotCorner::Server::Tracking {
	class TrayCornerTracker final : public TrayIcon {
		static constexpr uint32_t TrackMessage = 0x421;
		static constexpr uint32_t UntrackMessage = 0x406;

		TrayCornerTracker() noexcept;

	protected:
		LRESULT HandleMessage(
			UINT message,
			WPARAM wParam,
			LPARAM lParam) noexcept override final;

	public:
		static TrayCornerTracker& Current();

		CornerTracker::StartupResult BeginTracking() const noexcept;
		CornerTracker::StopResult StopTracking() const noexcept;
	};
}
