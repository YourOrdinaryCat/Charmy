#include "pch.h"
#include "LifetimeManager.h"
#include "Resources/resource.h"

namespace winrt::HotCorner::Server::implementation {
	LifetimeManager::LifetimeManager() noexcept :
		m_icon(Tracking::TrayCornerTracker::Current())
	{
		const auto tip = Resources::GetString<128>(Current::Module(), IDS_TRAY_TOOLTIP);
		m_icon.UpdateToolTip(tip.data());
	}

	void LifetimeManager::TrackHotCorners() noexcept {
		const auto result = m_icon.BeginTracking();

		if (result == CornerTracker::StartupResult::Started) {
			BumpServer();
		}
		else if (result == CornerTracker::StartupResult::Failed) {
			//TODO: Handle failure
			OutputDebugString(L"Failed to start corner tracking\n");
		}
	}

	void LifetimeManager::StopTracking() noexcept {
		const auto result = m_icon.StopTracking();

		if (result == CornerTracker::StopResult::Stopped) {
			ReleaseServer();
		}
		else if (result == CornerTracker::StopResult::Failed) {
			//TODO: Handle failure
			OutputDebugString(L"Failed to stop corner tracking\n");
		}
	}

	void LifetimeManager::ShowTrayIcon() noexcept {
		if (!m_icon.Visible()) {
			m_icon.Show();
			BumpServer();
		}
	}

	void LifetimeManager::HideTrayIcon() noexcept {
		if (m_icon.Visible()) {
			m_icon.Hide();
			ReleaseServer();
		}
	}
}
