#pragma once
#include <filesystem>
#include "SettingsManager.h"
#include "Tracking/TrayCornerTracker.h"

namespace winrt::HotCorner::Server {
	class App final {
		HINSTANCE m_instance;
		Settings::SettingsManager m_settings;
		Tracking::TrayCornerTracker m_tracker;

	public:
		inline App(HINSTANCE instance, const std::filesystem::path& roamingFolder) noexcept :
			m_instance(instance), m_settings(roamingFolder), m_tracker(instance, m_settings)
		{ }

		constexpr HINSTANCE Module() const noexcept {
			return m_instance;
		}

		constexpr Settings::SettingsManager& Settings() noexcept {
			return m_settings;
		}

		constexpr Tracking::TrayCornerTracker& TrayIcon() noexcept {
			return m_tracker;
		}

		inline int Run() const noexcept {
			return static_cast<int>(m_tracker.RunMessageLoop());
		}
	};
}
