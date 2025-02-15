#include "pch.h"
#include "AppSettings.h"

namespace ws = winrt::Windows::Storage;

namespace winrt::HotCorner::Uwp {
	hstring SettingsPath() {
		return ws::AppDataPaths::GetDefault().RoamingAppData();
	}

	ws::StorageFolder SettingsFolder() {
		return Windows::Storage::ApplicationData::Current().RoamingFolder();
	}

	Settings::SettingsManager m_settings{};
	Settings::SettingsManager& AppSettings() noexcept {
		return m_settings;
	}
}
