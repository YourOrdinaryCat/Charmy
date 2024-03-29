#pragma once
#include "SettingsManager.h"

namespace winrt::HotCorner::Server::Current {
	HINSTANCE Module() noexcept;

	/**
	 * @brief Gets the singleton instance of the current settings manager.
	*/
	Settings::SettingsManager& Settings() noexcept;
}
