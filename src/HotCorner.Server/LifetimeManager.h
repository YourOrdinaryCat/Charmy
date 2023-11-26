#pragma once
#include "LifetimeManager.g.h"
#include "Tracking/TrayCornerTracker.h"

import server;

// Implementation outside of the LifetimeManager, to allow usage within the
// server itself
namespace winrt::HotCorner::Server {
	void TrackHotCorners() noexcept;
	void StopTracking() noexcept;

	void ShowTrayIcon() noexcept;
	void HideTrayIcon() noexcept;

	inline void BumpServer() noexcept {
		server::add_ref();
	}

	inline void ReleaseServer() noexcept {
		if (server::release_ref() == 0) {
			Tracking::TrayCornerTracker::Current().Close();
		}
	}
}

namespace winrt::HotCorner::Server::implementation {
	struct
		__declspec(uuid("898F12B7-4BB0-4279-B3B1-126440D7CB7A"))
		LifetimeManager : LifetimeManagerT<LifetimeManager>
	{
		LifetimeManager() noexcept;

		void LockServer(uint32_t pid);
		void ReloadSettings();

		void TrackHotCorners() const noexcept;
		void StopTracking() const noexcept;

		void ShowTrayIcon() const noexcept;
		void HideTrayIcon() const noexcept;

		~LifetimeManager() noexcept;

	private:
		HANDLE m_waitHandle{};

		static void OnWaited(PVOID, BOOLEAN);
		winrt::fire_and_forget Unregister() noexcept;
	};
}
