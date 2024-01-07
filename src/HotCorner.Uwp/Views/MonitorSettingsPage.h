#pragma once
#include <AppSettings.h>
#include <Devices/MonitorInfo.h>
#include <Utils/Property.h>

#include "Views/MonitorSettingsPage.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	/**
	 * @brief Shown to the user when a monitor is picked.
	*/
	struct MonitorSettingsPage : MonitorSettingsPageT<MonitorSettingsPage> {
		MonitorSettingsPage() { }

		void InitializeComponent();

		/**
		 * @brief Updates the page to show the settings for the monitor with
		 *        the provided Id.
		*/
		void SetMonitorId(const hstring& id);

		event_token SettingRemoved(const SettingRemovedEventHandler& handler);
		void SettingRemoved(const event_token& token) noexcept;

		void OnDelayToggleChecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		void OnDelayToggleUnchecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		void OnGlobalToggleChecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);
		void OnGlobalToggleUnchecked(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		void OnRemoveConfigClick(const IInspectable&, const Windows::UI::Xaml::RoutedEventArgs&);

		//TODO: Refactor?
		void OnTopLeftActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnTopRightActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnBottomLeftActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);
		void OnBottomRightActionSelected(const IInspectable&, const Windows::UI::Xaml::Controls::SelectionChangedEventArgs&);

		void OnTopLeftDelayChanged(const Microsoft::UI::Xaml::Controls::NumberBox&, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs&);
		void OnTopRightDelayChanged(const Microsoft::UI::Xaml::Controls::NumberBox&, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs&);
		void OnBottomLeftDelayChanged(const Microsoft::UI::Xaml::Controls::NumberBox&, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs&);
		void OnBottomRightDelayChanged(const Microsoft::UI::Xaml::Controls::NumberBox&, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs&);

	private:
		static Windows::Foundation::Collections::IVectorView<IInspectable> CornerActions();

		event<SettingRemovedEventHandler> m_settingRemovedEvent;
		std::wstring m_currentId = L"";

		/**
		 * @brief Gets a reference to the settings for the currently selected
		 *        monitor.
		*/
		inline Settings::MonitorSettings& CurrentSettings() const noexcept {
			if (m_currentId.empty()) {
				return AppSettings().DefaultSettings;
			}
			return AppSettings().Monitors.at(m_currentId);
		}
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MonitorSettingsPage : MonitorSettingsPageT<MonitorSettingsPage, implementation::MonitorSettingsPage> {
	};
}
