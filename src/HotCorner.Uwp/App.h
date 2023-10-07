#pragma once
#include "App.xaml.g.h"

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
