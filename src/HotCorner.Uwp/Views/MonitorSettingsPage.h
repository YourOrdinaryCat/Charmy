#pragma once
#include <AppSettings.h>
#include <Devices/MonitorInfo.h>
#include <Utils/Xaml.h>

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

		void OnDelayToggleChecked(const IInspectable&, const wux::RoutedEventArgs&);
		void OnDelayToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&);

		void OnGlobalToggleChecked(const IInspectable&, const wux::RoutedEventArgs&);
		void OnGlobalToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&);

		void OnRemoveConfigClick(const IInspectable&, const wux::RoutedEventArgs&);

		//TODO: Refactor?
		void OnTopLeftActionSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&);
		void OnTopRightActionSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&);
		void OnBottomLeftActionSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&);
		void OnBottomRightActionSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&);

		void OnTopLeftDelayChanged(const muxc::NumberBox&, const muxc::NumberBoxValueChangedEventArgs&);
		void OnTopRightDelayChanged(const muxc::NumberBox&, const muxc::NumberBoxValueChangedEventArgs&);
		void OnBottomLeftDelayChanged(const muxc::NumberBox&, const muxc::NumberBoxValueChangedEventArgs&);
		void OnBottomRightDelayChanged(const muxc::NumberBox&, const muxc::NumberBoxValueChangedEventArgs&);

	private:
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
