#pragma once
#include <SettingsManager.h>
#include <winrt/Windows.Storage.h>

namespace winrt::HotCorner::Uwp {
	const Windows::Storage::StorageFolder SettingsFolder =
		Windows::Storage::ApplicationData::Current().RoamingFolder();

	/**
	 * @brief Gets the singleton instance of the app's settings manager.
	*/
	Settings::SettingsManager& AppSettings();
}
