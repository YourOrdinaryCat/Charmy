#include "pch.h"
#include "LifetimeManager.h"

#include "main.h"
#include "Resources/resource.h"

import server;

namespace winrt::HotCorner::Server::implementation {
	LifetimeManager::LifetimeManager() noexcept {
		const auto tip = Resources::GetString<128>(Current::Module(), IDS_TRAY_TOOLTIP);
		Current::Notification().UpdateToolTip(tip.data());
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
			if (server::release_ref() == 0) {
				Current::Notification().Close();
			}

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
