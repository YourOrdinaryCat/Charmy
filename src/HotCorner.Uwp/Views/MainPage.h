#pragma once
#include "MonitorSettingsPage.h"
#include <Devices/MonitorInfo.h>
#include <Devices/Watcher.h>
#include <winrt/Windows.Devices.Display.h>

#include "Views/MainPage.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	/**
	 * @brief Shown to the user when the app is launched normally
	 *        or through the tray icon.
	*/
	struct MainPage : MainPageT<MainPage> {
		MainPage();

		void InitializeComponent();

		//TODO: On click, the button should transform into a start/stop button
		void OnStartStopButtonClick(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		void OnApplyButtonClick(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		winrt::fire_and_forget OnOKButtonClick(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		winrt::fire_and_forget OnCancelButtonClick(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		void OnMonitorSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);

	private:
		Devices::Watcher<Windows::Devices::Display::DisplayMonitor, Devices::MonitorInfo> m_watcher{};
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MainPage : MainPageT<MainPage, implementation::MainPage> {
	};
}
