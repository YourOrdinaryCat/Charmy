#pragma once
#include "App.xaml.g.h"
#include <SettingsManager.h>
#include <winrt/Windows.ApplicationModel.Activation.h>

namespace winrt::HotCorner::Uwp::implementation {
	struct App : AppT<App> {
		/**
		 * @brief Creates the singleton application object.
		*/
		App();

		/**
		 * @brief Invoked when the application is launched normally by the end user.
		 *
		 * @param args Details about the launch request and process.
		*/
		void OnLaunched(const Windows::ApplicationModel::Activation::LaunchActivatedEventArgs&) const;
	};
}

namespace winrt::HotCorner::Uwp {
	/**
	 * @brief Gets the singleton instance of the app's settings manager.
	*/
	Settings::SettingsManager& AppSettings();
}
