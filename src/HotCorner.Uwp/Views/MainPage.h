#pragma once

#include "Views/MainPage.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	/**
	 * @brief Shown to the user when the app is launched normally
	 *        or through the tray icon.
	*/
	struct MainPage : MainPageT<MainPage> {
		MainPage() {
			// Xaml objects should not call InitializeComponent during construction.
			// See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
		}

		void StartHandler(const Windows::Foundation::IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		void StopHandler(const Windows::Foundation::IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MainPage : MainPageT<MainPage, implementation::MainPage> {
	};
}
