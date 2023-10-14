#pragma once
#include "LifetimeManager.g.h"

#include "Tray/TrayIcon.h"

namespace winrt::HotCorner::Server::implementation {
	struct
		__declspec(uuid("898F12B7-4BB0-4279-B3B1-126440D7CB7A"))
		LifetimeManager : LifetimeManagerT<LifetimeManager>
	{
		LifetimeManager() noexcept;

		void TrackHotCorners() noexcept;
		void StopTracking() noexcept;

		void ShowTrayIcon() noexcept;
		void HideTrayIcon() noexcept;

	private:
		bool m_tracking = false;
	};
}
