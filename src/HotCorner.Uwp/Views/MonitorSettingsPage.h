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
		Settings::MonitorSettings& CurrentSettings() const;

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

		void OnActionSelected(const Windows::UI::Xaml::Controls::ComboBox&, Settings::CornerAction&) const;

		//TODO: Refactor?
		void OnTopLeftActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnTopRightActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnBottomLeftActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnBottomRightActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);

		DEPENDENCY_PROPERTY_META(Monitor, Devices::MonitorInfo, nullptr, OnMonitorPropertyChanged);
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MonitorSettingsPage : MonitorSettingsPageT<MonitorSettingsPage, implementation::MonitorSettingsPage> {
	};
}
