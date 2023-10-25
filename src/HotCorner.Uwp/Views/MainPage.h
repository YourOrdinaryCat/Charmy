#pragma once
#include "Views/MainPage.g.h"

#include "../Devices/MonitorInfo.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	/**
	 * @brief Shown to the user when the app is launched normally
	 *        or through the tray icon.
	*/
	struct MainPage : MainPageT<MainPage> {
		MainPage();
		~MainPage() noexcept;

		void InitializeComponent();

		const Windows::System::DispatcherQueue DispatcherQueue;

		//TODO: On click, the button should transform into a start/stop button
		void OnStartStopButtonClick(const Windows::Foundation::IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		//TODO: On click, save user preferences and exit
		winrt::fire_and_forget OnOKButtonClick(const Windows::Foundation::IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		winrt::fire_and_forget OnCancelButtonClick(const Windows::Foundation::IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

	private:
		/**
		 * @brief Gets the index of a monitor in m_monitors based on the provided monitor ID.
		 *
		 * @returns The index if the monitor was found, nullopt otherwise.
		*/
		std::optional<uint32_t> TryGetMonitorIndex(const hstring& id) const noexcept;

		const Windows::Devices::Enumeration::DeviceWatcher m_watcher;
		const Windows::Foundation::Collections::IObservableVector<Devices::MonitorInfo> m_monitors;

		const winrt::event_token m_addToken;
		const winrt::event_token m_removeToken;
		const winrt::event_token m_updateToken;
		const winrt::event_token m_stoppedToken;

		winrt::fire_and_forget OnDeviceAdded(
			const Windows::Devices::Enumeration::DeviceWatcher&,
			const Windows::Devices::Enumeration::DeviceInformation&
		);

		winrt::fire_and_forget OnDeviceRemoved(
			const Windows::Devices::Enumeration::DeviceWatcher&,
			const Windows::Devices::Enumeration::DeviceInformationUpdate&
		);

		winrt::fire_and_forget OnDeviceUpdated(
			const Windows::Devices::Enumeration::DeviceWatcher&,
			const Windows::Devices::Enumeration::DeviceInformationUpdate&
		);

		//TODO: Notify the user when the watcher stops, they'll want to restart it
		void OnDeviceEnumerationStopped(
			const Windows::Devices::Enumeration::DeviceWatcher&,
			const Windows::Foundation::IInspectable&
		);
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MainPage : MainPageT<MainPage, implementation::MainPage> {
	};
}
