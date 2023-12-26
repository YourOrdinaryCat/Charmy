#pragma once
#include "MonitorSettingsPage.h"
#include <App.h>
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
		MainPage() { }

		void InitializeComponent();

		void OnGlobalToggleChecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		void OnGlobalToggleUnchecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		void OnTrayIconToggleChecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		void OnTrayIconToggleUnchecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		void OnApplyButtonClick(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		winrt::fire_and_forget OnOKButtonClick(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		winrt::fire_and_forget OnCancelButtonClick(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		void OnMonitorSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);

	private:
		void Save() const;

		Devices::Watcher<Windows::Devices::Display::DisplayMonitor, Devices::MonitorInfo> m_watcher{};
		Settings::SettingsManager& m_settings = Uwp::implementation::App::Settings();
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MainPage : MainPageT<MainPage, implementation::MainPage> {
	};
}
