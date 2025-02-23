#pragma once
#include "LifetimeManager.g.h"
#include "App.h"
#include "Tracking/TrayCornerTracker.h"

// Implementation outside of the LifetimeManager, to allow usage within the
// server itself
namespace winrt::HotCorner::Server {
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
	struct LifetimeManager : LifetimeManagerT<LifetimeManager> {
		LifetimeManager(App& app) noexcept;

		void ReloadSettings();

		Windows::Foundation::IAsyncAction BeginTrackingAsync() const;
		Windows::Foundation::IAsyncAction StopTrackingAsync() const;

		void ShowTrayIcon() const noexcept;
		void HideTrayIcon() const noexcept;

		~LifetimeManager() noexcept;

	private:
		App& m_app;
	};
}
