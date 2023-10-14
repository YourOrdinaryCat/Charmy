#include "pch.h"
#include "LifetimeManager.h"

#include "main.h"

import server;

namespace winrt::HotCorner::Server::implementation {
	LifetimeManager::LifetimeManager() noexcept {
		Current::Notification().UpdateToolTip(L"HotCorner");
	}

	void LifetimeManager::TrackHotCorners() noexcept {
		if (!m_tracking) {
			m_tracking = true;
			server::add_ref();

			//TODO: Actually implement hot corners
		}
	}

	void LifetimeManager::StopTracking() noexcept {
		if (m_tracking) {
			m_tracking = false;
			server::release_ref();

			//TODO: Actually implement hot corners
		}
	}

	void LifetimeManager::ShowTrayIcon() noexcept {
		Current::Notification().Show();
	}

	void LifetimeManager::HideTrayIcon() noexcept {
		Current::Notification().Hide();
	}
}
