#include "pch.h"
#include "LifetimeManager.h"
#include <Logging.h>

namespace wf = winrt::Windows::Foundation;

namespace winrt::HotCorner::Server {
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
		m_app.LoadSettings();
		Logging::FileSink()->set_level(m_app.Settings().LogVerbosity);
	}

	wf::IAsyncAction LifetimeManager::BeginTrackingAsync() const {
		co_await m_app.TrayIcon().BeginTrackingAsync();
	}
	wf::IAsyncAction LifetimeManager::StopTrackingAsync() const {
		co_await m_app.TrayIcon().StopTrackingAsync();
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
