#pragma once
#include "MonitorSettingsPage.h"
#include <AppSettings.h>
#include <Controls/EvenStackPanel.h>
#include <Controls/WrapPanel.h>
#include <Devices/MonitorInfo.h>
#include <Devices/Watcher.h>
#include <Utils/Xaml.h>
#include <winrt/Windows.Devices.Display.h>
#include <winrt/Windows.System.h>

#include "Views/MainPage.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	/**
	 * @brief Shown to the user when the app is launched normally
	 *        or through the tray icon.
	*/
	struct MainPage : MainPageT<MainPage> {
		MainPage() {}

		winrt::fire_and_forget InitializeComponent();

		void OnGlobalCheckLoaded(const IInspectable&, const wux::RoutedEventArgs&);
		void OnTrayIconCheckLoaded(const IInspectable&, const wux::RoutedEventArgs&);

		winrt::fire_and_forget OnAddButtonClick(const IInspectable&, const wux::RoutedEventArgs&);
		void OnMonitorClick(const IInspectable&, const wux::RoutedEventArgs&);
		void OnSettingRemoved(const hstring&);

		void OnApplyButtonClick(const IInspectable&, const wux::RoutedEventArgs&);
		winrt::fire_and_forget OnOKButtonClick(const IInspectable&, const wux::RoutedEventArgs&);
		winrt::fire_and_forget OnCancelButtonClick(const IInspectable&, const wux::RoutedEventArgs&);

		void OnMonitorSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&);
		void OnCopyVersionClick(const IInspectable&, const wux::RoutedEventArgs&);
		winrt::fire_and_forget OnOpenLogFolderClick(const IInspectable&, const wux::RoutedEventArgs&);
		void OnLogVerbosityFlyoutOpening(const IInspectable&, const IInspectable&);
		void OnLogLevelClick(const IInspectable&, const wux::RoutedEventArgs&);

	private:
		void Save() const;

		Devices::Watcher<Devices::MonitorInfo> m_watcher{
			Windows::Devices::Display::DisplayMonitor::GetDeviceSelector(),
			Windows::System::DispatcherQueue::GetForCurrentThread(),
			Devices::DeviceWatcherEvent::None
		};
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MainPage : MainPageT<MainPage, implementation::MainPage> {
	};
}
