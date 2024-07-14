#include "pch.h"
#include "AppSettings.h"

namespace winrt::HotCorner::Uwp {
	Settings::SettingsManager& AppSettings() {
		static Settings::SettingsManager m_settings{ SettingsFolder.Path().c_str() };
		return m_settings;
	}
}
