#include "pch.h"
#include "AppSettings.h"

namespace winrt::HotCorner::Uwp {
	Settings::SettingsManager& AppSettings() {
		static Settings::SettingsManager m_settings{ SettingsPath };
		return m_settings;
	}
}
