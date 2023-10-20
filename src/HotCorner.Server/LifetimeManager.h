#pragma once
#include "LifetimeManager.g.h"
#include "Tracking/TrayCornerTracker.h"

import server;

namespace winrt::HotCorner::Server::implementation {
	struct
		__declspec(uuid("898F12B7-4BB0-4279-B3B1-126440D7CB7A"))
		LifetimeManager : LifetimeManagerT<LifetimeManager>
	{
		LifetimeManager() noexcept;

		void LockServer(uint32_t pid);

		void TrackHotCorners() const noexcept;
		void StopTracking() const noexcept;

		void ShowTrayIcon() const noexcept;
		void HideTrayIcon() const noexcept;

		~LifetimeManager() noexcept;

	private:
		Tracking::TrayCornerTracker& m_icon;
		HANDLE m_waitHandle{};

		static void OnWaited(PVOID, BOOLEAN);
		winrt::fire_and_forget Unregister() noexcept;

		inline void BumpServer() const noexcept {
			server::add_ref();
		}

		inline void ReleaseServer() const noexcept {
			if (server::release_ref() == 0) {
				m_icon.Close();
			}
		}
	};
}
