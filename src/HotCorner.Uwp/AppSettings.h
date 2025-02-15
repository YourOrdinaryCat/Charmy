#pragma once
#include <SettingsManager.h>
#include <winrt/Windows.Storage.h>

namespace winrt::HotCorner::Uwp {
	hstring SettingsPath();

	Windows::Storage::StorageFolder SettingsFolder();

	/**
	 * @brief Gets the singleton instance of the app's settings manager.
	*/
	Settings::SettingsManager& AppSettings() noexcept;
}
