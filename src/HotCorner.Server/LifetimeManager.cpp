#include "pch.h"
#include "LifetimeManager.h"
#include <Logging.h>

namespace winrt::HotCorner::Server {
	void TrackHotCorners(const Tracking::TrayCornerTracker& tct) noexcept {
		const auto result = tct.BeginTracking();

		if (result == StartupResult::Started) {
			BumpServer();
		}
		else if (result == StartupResult::Failed) {
			SPDLOG_LAST_ERROR(spdlog::level::err, "Failed to start corner tracking");
		}
	}

	void StopTracking(Tracking::TrayCornerTracker& tct) noexcept {
		const auto result = tct.StopTracking();

		if (result == StopResult::Stopped) {
			ReleaseServer(tct);
		}
		else if (result == StopResult::Failed) {
			SPDLOG_LAST_ERROR(spdlog::level::err, "Failed to stop corner tracking");
		}
	}

	void ShowTrayIcon(Tracking::TrayCornerTracker& tct) noexcept {
		if (!tct.Visible()) {
			tct.Show();
			BumpServer();
		}
	}

	void HideTrayIcon(Tracking::TrayCornerTracker& tct) noexcept {
		if (tct.Visible()) {
			tct.Hide();
			ReleaseServer(tct);
		}
	}
}

namespace winrt::HotCorner::Server::implementation {
	LifetimeManager::LifetimeManager(App& app) noexcept : m_app(app) {
		BumpServer();
	}

	void LifetimeManager::ReloadSettings() {
		m_app.Settings().Load();
		Logging::FileSink()->set_level(m_app.Settings().LogVerbosity);
	}

	void LifetimeManager::TrackHotCorners() const noexcept {
		Server::TrackHotCorners(m_app.TrayIcon());
	}
	void LifetimeManager::StopTracking() const noexcept {
		Server::StopTracking(m_app.TrayIcon());
	}
	void LifetimeManager::ShowTrayIcon() const noexcept {
		Server::ShowTrayIcon(m_app.TrayIcon());
	}
	void LifetimeManager::HideTrayIcon() const noexcept {
		Server::HideTrayIcon(m_app.TrayIcon());
	}

	LifetimeManager::~LifetimeManager() noexcept {
		ReleaseServer(m_app.TrayIcon());
	}
}
