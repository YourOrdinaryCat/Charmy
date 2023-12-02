#pragma once
#include <App.h>
#include <Devices/MonitorInfo.h>
#include <Utils/Property.h>

#include "Views/MonitorSettingsPage.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	/**
	 * @brief Shown to the user when a monitor is picked.
	*/
	struct MonitorSettingsPage : MonitorSettingsPageT<MonitorSettingsPage> {
	private:
		static Windows::Foundation::Collections::IVectorView<IInspectable> CornerActions();

		static void OnMonitorPropertyChanged(
			const Windows::UI::Xaml::DependencyObject&,
			const Windows::UI::Xaml::DependencyPropertyChangedEventArgs&
		);

		size_t m_currentSettings = 0;

		/**
		 * @brief Gets the settings for the currently selected monitor.
		*/
		inline Settings::MonitorSettings& CurrentSettings() const noexcept;

		/**
		 * @brief Refreshes the page's data with the current settings.
		*/
		void Refresh(size_t index);

	public:
		MonitorSettingsPage() {

		}

		void InitializeComponent();

		void OnDelayToggleChecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		void OnDelayToggleUnchecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		void OnGlobalToggleChecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		void OnGlobalToggleUnchecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		//TODO: Refactor?
		void OnTopLeftActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnTopRightActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnBottomLeftActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnBottomRightActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);

		void OnTopLeftDelayChanged(const Microsoft::UI::Xaml::Controls::NumberBox&, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs&);
		void OnTopRightDelayChanged(const Microsoft::UI::Xaml::Controls::NumberBox&, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs&);
		void OnBottomLeftDelayChanged(const Microsoft::UI::Xaml::Controls::NumberBox&, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs&);
		void OnBottomRightDelayChanged(const Microsoft::UI::Xaml::Controls::NumberBox&, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs&);

		DEPENDENCY_PROPERTY_META(Monitor, Devices::MonitorInfo, nullptr, OnMonitorPropertyChanged);
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MonitorSettingsPage : MonitorSettingsPageT<MonitorSettingsPage, implementation::MonitorSettingsPage> {
	};
}
