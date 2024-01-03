#include "pch.h"
#include "MonitorSettingsPage.h"
#include "Views/MonitorSettingsPage.g.cpp"

namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;

using App = winrt::HotCorner::Uwp::implementation::App;

namespace winrt::HotCorner::Uwp::Views::implementation {
	//TODO: A better way of doing this? Or at least move it to its own header?
	static IInspectable operator ""_box(const wchar_t* str, std::size_t size) {
		return box_value(hstring{ { str, size } });
	}

	wfc::IVectorView<IInspectable> MonitorSettingsPage::CornerActions() {
		static const auto m_cornerActions = winrt::single_threaded_vector<IInspectable>(
			{ L"None"_box, L"Open Task View"_box, L"Open Start"_box, L"Open Search"_box, L"Go to Desktop"_box }
		).GetView();
		return m_cornerActions;
	}

	static void UpdateSelection(const wuxc::ComboBox& box, const Settings::CornerAction act) {
		if (act != Settings::CornerAction::None) {
			box.SelectedIndex(static_cast<int32_t>(act));
		}
		else {
			box.SelectedIndex(-1);
		}
	}

	void MonitorSettingsPage::Refresh(const std::wstring& id) {
		m_currentId = id;
		const auto& setting = CurrentSettings();

		GlobalCheck().IsChecked(setting.Enabled);
		DelayCheck().IsChecked(setting.DelayEnabled);

		UpdateSelection(TopLeftCorner(), setting.TopLeftAction);
		UpdateSelection(TopRightCorner(), setting.TopRightAction);
		UpdateSelection(BottomLeftCorner(), setting.BottomLeftAction);
		UpdateSelection(BottomRightCorner(), setting.BottomRightAction);

		TopLeftDelay().Value(static_cast<double>(setting.TopLeftDelay));
		TopRightDelay().Value(static_cast<double>(setting.TopRightDelay));
		BottomLeftDelay().Value(static_cast<double>(setting.BottomLeftDelay));
		BottomRightDelay().Value(static_cast<double>(setting.BottomRightDelay));
	}

	void MonitorSettingsPage::InitializeComponent() {
		MonitorSettingsPageT::InitializeComponent();
		const auto items = CornerActions();

		TopLeftCorner().ItemsSource(items);
		TopRightCorner().ItemsSource(items);
		BottomLeftCorner().ItemsSource(items);
		BottomRightCorner().ItemsSource(items);
	}

	void MonitorSettingsPage::OnGlobalToggleChecked(const IInspectable&, const wux::RoutedEventArgs&) {
		CurrentSettings().Enabled = true;
	}
	void MonitorSettingsPage::OnGlobalToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&) {
		CurrentSettings().Enabled = false;
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
