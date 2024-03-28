#include "pch.h"
#include "AppSettings.h"
#include <winrt/Windows.Storage.h>

namespace winrt::HotCorner::Uwp {
	Settings::SettingsManager& AppSettings() {
		static Settings::SettingsManager m_settings{
			Windows::Storage::ApplicationData::Current().RoamingFolder().Path().c_str()
		};
		return m_settings;
	}
}
