#pragma once
#include <AppSettings.h>
#include <Controls/EvenStackPanel.h>
#include <Devices/MonitorInfo.h>
#include <Devices/Watcher.h>
#include <Utils/Xaml.h>
#include <winrt/Windows.Devices.Display.h>

#include "MonitorSettingsPage.h"
#include "Views/MainPage.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	using namespace XamlAliases;

	/**
	 * @brief Shown to the user when the app is launched normally
	 *        or through the tray icon.
	*/
	struct MainPage : MainPageT<MainPage> {
		MainPage() { }

		void InitializeComponent();

		void OnGlobalToggleChecked(const IInspectable&, const wux::RoutedEventArgs&);
		void OnGlobalToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&);

		void OnTrayIconToggleChecked(const IInspectable&, const wux::RoutedEventArgs&);
		void OnTrayIconToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&);

		void OnSettingAdded(const hstring&, const hstring&);
		void OnSettingRemoved(const hstring&);

		void OnApplyButtonClick(const IInspectable&, const wux::RoutedEventArgs&);
		winrt::fire_and_forget OnOKButtonClick(const IInspectable&, const wux::RoutedEventArgs&);
		winrt::fire_and_forget OnCancelButtonClick(const IInspectable&, const wux::RoutedEventArgs&);

		void OnMonitorSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&);

	private:
		void Save() const;

		Devices::Watcher<Devices::MonitorInfo> m_watcher{
			Windows::Devices::Display::DisplayMonitor::GetDeviceSelector(),
			Devices::DeviceWatcherEvent::None
		};
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MainPage : MainPageT<MainPage, implementation::MainPage> {
	};
}
