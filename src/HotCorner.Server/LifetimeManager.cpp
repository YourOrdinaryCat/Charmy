#include "pch.h"
#include "LifetimeManager.h"
#include "main.h"

namespace winrt::HotCorner::Server {
	using TCT = Tracking::TrayCornerTracker;

	void TrackHotCorners() noexcept {
		const auto result = TCT::Current().BeginTracking();

		if (result == CornerTracker::StartupResult::Started) {
			BumpServer();
		}
		else if (result == CornerTracker::StartupResult::Failed) {
			//TODO: Handle failure
			OutputDebugString(L"Failed to start corner tracking\n");
		}
	}

	void StopTracking() noexcept {
		const auto result = TCT::Current().StopTracking();

		if (result == CornerTracker::StopResult::Stopped) {
			ReleaseServer();
		}
		else if (result == CornerTracker::StopResult::Failed) {
			//TODO: Handle failure
			OutputDebugString(L"Failed to stop corner tracking\n");
		}
	}

	void ShowTrayIcon() noexcept {
		auto& icon = TCT::Current();
		if (!icon.Visible()) {
			icon.Show();
			BumpServer();
		}
	}

	void HideTrayIcon() noexcept {
		auto& icon = TCT::Current();
		if (icon.Visible()) {
			icon.Hide();
			ReleaseServer();
		}
	}
}

namespace winrt::HotCorner::Server::implementation {
	LifetimeManager::LifetimeManager() noexcept {
		BumpServer();
	}

	void LifetimeManager::ReloadSettings() {
		const auto result = TCT::Current().StopTracking();
		Current::Settings().Load();

		if (result == CornerTracker::StopResult::Stopped) {
			TCT::Current().BeginTracking();
		}
		else if (result == CornerTracker::StopResult::Failed) {
			//TODO: Handle failure
			OutputDebugString(L"Failed to stop corner tracking\n");
		}
	}

	void LifetimeManager::TrackHotCorners() const noexcept {
		Server::TrackHotCorners();
	}
	void LifetimeManager::StopTracking() const noexcept {
		Server::StopTracking();
	}
	void LifetimeManager::ShowTrayIcon() const noexcept {
		Server::ShowTrayIcon();
	}
	void LifetimeManager::HideTrayIcon() const noexcept {
		Server::HideTrayIcon();
	}

	LifetimeManager::~LifetimeManager() noexcept {
		ReleaseServer();
	}
}
