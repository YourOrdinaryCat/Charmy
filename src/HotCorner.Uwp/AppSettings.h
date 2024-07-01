#pragma once
#include <SettingsManager.h>
#include <winrt/Windows.Storage.h>

namespace winrt::HotCorner::Uwp {
	const std::filesystem::path SettingsPath{
		Windows::Storage::ApplicationData::Current().RoamingFolder().Path().c_str()
	};

	/**
	 * @brief Gets the singleton instance of the app's settings manager.
	*/
	Settings::SettingsManager& AppSettings();
}
