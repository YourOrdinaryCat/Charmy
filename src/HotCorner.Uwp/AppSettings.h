#pragma once
#include <SettingsManager.h>

namespace winrt::HotCorner::Uwp {
	/**
	 * @brief Gets the singleton instance of the app's settings manager.
	*/
	Settings::SettingsManager& AppSettings();
}
