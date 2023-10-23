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

		//TODO: On click, the button should transform into a start/stop button
		void OnStartStopButtonClick(const Windows::Foundation::IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		//TODO: On click, save user preferences and exit
		winrt::fire_and_forget OnOKButtonClick(const Windows::Foundation::IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		winrt::fire_and_forget OnCancelButtonClick(const Windows::Foundation::IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MainPage : MainPageT<MainPage, implementation::MainPage> {
	};
}
