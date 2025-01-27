#pragma once
#include "App.base.h"

#include <winrt/Windows.ApplicationModel.Activation.h>

namespace winrt::HotCorner::Uwp::implementation {
	struct App : AppT2<App> {
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

namespace winrt::HotCorner::Uwp::factory_implementation {
	class App : public AppT<App, implementation::App> {
	};
}
