#pragma once
#include "LifetimeManager.g.h"
#include "App.h"
#include "Tracking/TrayCornerTracker.h"

// Implementation outside of the LifetimeManager, to allow usage within the
// server itself
namespace winrt::HotCorner::Server {
	void TrackHotCorners(const Tracking::TrayCornerTracker&) noexcept;
	void StopTracking(Tracking::TrayCornerTracker&) noexcept;

	void ShowTrayIcon(Tracking::TrayCornerTracker&) noexcept;
	void HideTrayIcon(Tracking::TrayCornerTracker&) noexcept;

	inline void BumpServer() noexcept {
		CoAddRefServerProcess();
	}

	inline void ReleaseServer(Tracking::TrayCornerTracker& tct) noexcept {
		if (CoReleaseServerProcess() == 0) {
			tct.Close();
		}
	}
}

namespace winrt::HotCorner::Server::implementation {
	struct
		__declspec(uuid("898F12B7-4BB0-4279-B3B1-126440D7CB7A"))
		LifetimeManager : LifetimeManagerT<LifetimeManager>
	{
		LifetimeManager(App& app) noexcept;

		void ReloadSettings();

		void TrackHotCorners() const noexcept;
		void StopTracking() const noexcept;

		void ShowTrayIcon() const noexcept;
		void HideTrayIcon() const noexcept;

		~LifetimeManager() noexcept;

	private:
		App& m_app;
	};
}
