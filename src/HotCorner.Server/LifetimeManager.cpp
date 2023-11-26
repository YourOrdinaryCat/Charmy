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
	LifetimeManager::LifetimeManager() noexcept { }

	void LifetimeManager::OnWaited(PVOID param, BOOLEAN) {
		static_cast<LifetimeManager*>(param)->Unregister();
	}

	winrt::fire_and_forget LifetimeManager::Unregister() noexcept {
		co_await std::chrono::seconds(0);
		ReleaseServer();

		Release();
	}

	void LifetimeManager::LockServer(uint32_t pid) {
		const auto proc = OpenProcess(SYNCHRONIZE, false, pid);

		if (!proc) {
			throw winrt::hresult_invalid_argument(L"The provided process ID is invalid.");
		}

		const bool registered = RegisterWaitForSingleObject(
			&m_waitHandle,
			proc,
			&LifetimeManager::OnWaited,
			this,
			INFINITE,
			WT_EXECUTEONLYONCE
		);

		winrt::check_bool(registered);
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
		if (m_waitHandle) {
			const auto unregistered = UnregisterWait(m_waitHandle);
			if (!unregistered) {
				//TODO: Handle failure
				OutputDebugString(L"Failed to unregister wait handle\n");
			}
		}
	}
}
