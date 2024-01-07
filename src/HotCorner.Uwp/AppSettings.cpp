#include "pch.h"
#include "AppSettings.h"
#include <winrt/Windows.Storage.h>

namespace winrt::HotCorner::Uwp {
	Settings::SettingsManager& AppSettings() {
		static Settings::SettingsManager m_settings{
				Windows::Storage::ApplicationData::Current().LocalFolder().Path().c_str()
		};
		static bool m_settingsLoaded = false;

		if (!m_settingsLoaded) {
			m_settingsLoaded = true;
			m_settings.Load();
		}

		return m_settings;
	}
}
