﻿#include "pch.h"
#include "MonitorSettingsPage.h"
#include "Views/MonitorSettingsPage.g.cpp"

#include <Localization.h>

namespace wfc = winrt::Windows::Foundation::Collections;

namespace winrt::HotCorner::Uwp::Views::implementation {
	static wfc::IVectorView<IInspectable> CornerActions() {
		static const auto m_cornerActions = single_threaded_vector<IInspectable>({
			box_value(StringLoader().GetString(L"CornerActionNone")),
			box_value(StringLoader().GetString(L"CornerActionTaskView")),
			box_value(StringLoader().GetString(L"CornerActionStart")),
			box_value(StringLoader().GetString(L"CornerActionSearch")),
			box_value(StringLoader().GetString(L"CornerActionGoToDesktop")),
			box_value(StringLoader().GetString(L"CornerActionQuickSettings")),
			box_value(StringLoader().GetString(L"CornerActionPreviousVirtualDesktop")),
			box_value(StringLoader().GetString(L"CornerActionNextVirtualDesktop"))
		});
		return m_cornerActions.GetView();
	}

	static void UpdateSelection(const wuxc::ComboBox& box, const Settings::CornerAction act) {
		if (act != Settings::CornerAction::None) {
			box.SelectedIndex(static_cast<int32_t>(act));
		}
		else {
			box.SelectedIndex(-1);
		}
	}

	void MonitorSettingsPage::InitializeComponent() {
		MonitorSettingsPageT::InitializeComponent();
		const auto items = CornerActions();

		TopLeftCorner().ItemsSource(items);
		TopRightCorner().ItemsSource(items);
		BottomLeftCorner().ItemsSource(items);
		BottomRightCorner().ItemsSource(items);
	}

	void MonitorSettingsPage::SetMonitorId(const hstring& id) {
		m_currentId = std::wstring{ id };
		const auto& setting = CurrentSettings();

		GlobalCheck().IsChecked(setting.Enabled);
		DelayCheck().IsChecked(setting.DelayEnabled);
		RemoveConfigButton().Visibility(
			id.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible
		);

		UpdateSelection(TopLeftCorner(), setting.TopLeftAction);
		UpdateSelection(TopRightCorner(), setting.TopRightAction);
		UpdateSelection(BottomLeftCorner(), setting.BottomLeftAction);
		UpdateSelection(BottomRightCorner(), setting.BottomRightAction);

		TopLeftDelay().Value(static_cast<double>(setting.TopLeftDelay));
		TopRightDelay().Value(static_cast<double>(setting.TopRightDelay));
		BottomLeftDelay().Value(static_cast<double>(setting.BottomLeftDelay));
		BottomRightDelay().Value(static_cast<double>(setting.BottomRightDelay));
	}

	event_token MonitorSettingsPage::SettingRemoved(const SettingRemovedEventHandler& handler) {
		return m_settingRemovedEvent.add(handler);
	}
	void MonitorSettingsPage::SettingRemoved(const event_token& token) noexcept {
		return m_settingRemovedEvent.remove(token);
	}

	void MonitorSettingsPage::OnGlobalToggleChecked(const IInspectable&, const wux::RoutedEventArgs&) {
		CurrentSettings().Enabled = true;
	}
	void MonitorSettingsPage::OnGlobalToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&) {
		CurrentSettings().Enabled = false;
	}

	void MonitorSettingsPage::OnRemoveConfigClick(const IInspectable&, const wux::RoutedEventArgs&) {
		AppSettings().Monitors.erase(m_currentId);
		m_settingRemovedEvent(hstring{ m_currentId });
	}

	void MonitorSettingsPage::OnDelayToggleChecked(const IInspectable&, const wux::RoutedEventArgs&) {
		CurrentSettings().DelayEnabled = true;
	}
	void MonitorSettingsPage::OnDelayToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&) {
		CurrentSettings().DelayEnabled = false;
	}

	static void OnActionSelected(const wuxc::ComboBox& box, Settings::CornerAction& action) {
		const auto index = box.SelectedIndex();

		const auto newAction = index < 1 ?
			Settings::CornerAction::None : static_cast<Settings::CornerAction>(index);
		action = newAction;
	}

	void MonitorSettingsPage::OnTopLeftActionSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&) {
		OnActionSelected(TopLeftCorner(), CurrentSettings().TopLeftAction);
	}
	void MonitorSettingsPage::OnTopRightActionSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&) {
		OnActionSelected(TopRightCorner(), CurrentSettings().TopRightAction);
	}
	void MonitorSettingsPage::OnBottomLeftActionSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&) {
		OnActionSelected(BottomLeftCorner(), CurrentSettings().BottomLeftAction);
	}
	void MonitorSettingsPage::OnBottomRightActionSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&) {
		OnActionSelected(BottomRightCorner(), CurrentSettings().BottomRightAction);
	}

	void MonitorSettingsPage::OnTopLeftDelayChanged(const muxc::NumberBox&, const muxc::NumberBoxValueChangedEventArgs& e) {
		CurrentSettings().TopLeftDelay = static_cast<uint32_t>(e.NewValue());
	}
	void MonitorSettingsPage::OnTopRightDelayChanged(const muxc::NumberBox&, const muxc::NumberBoxValueChangedEventArgs& e) {
		CurrentSettings().TopRightDelay = static_cast<uint32_t>(e.NewValue());
	}
	void MonitorSettingsPage::OnBottomLeftDelayChanged(const muxc::NumberBox&, const muxc::NumberBoxValueChangedEventArgs& e) {
		CurrentSettings().BottomLeftDelay = static_cast<uint32_t>(e.NewValue());
	}
	void MonitorSettingsPage::OnBottomRightDelayChanged(const muxc::NumberBox&, const muxc::NumberBoxValueChangedEventArgs& e) {
		CurrentSettings().BottomRightDelay = static_cast<uint32_t>(e.NewValue());
	}
}
