#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"

#include <Localization.h>
#include <Server/Lifetime.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace wuvm = winrt::Windows::UI::ViewManagement;

using winrt::HotCorner::Uwp::Devices::MonitorInfo;

namespace winrt::HotCorner::Uwp::Views::implementation {
	void MainPage::InitializeComponent() {
		MainPageT::InitializeComponent();

		GlobalCheck().IsChecked(AppSettings().TrackingEnabled);
		TrayIconCheck().IsChecked(AppSettings().TrayIconEnabled);

		const auto connected = m_watcher.ConnectedDevices();

		connected.Append({ L"", StringLoader().GetString(L"DefaultSettings") });
		for (const auto& setting : AppSettings().Monitors) {
			connected.Append({ setting.first, setting.second.DisplayName });
		}

		MonitorPicker().ItemsSource(connected);
		m_watcher.Start();
	}

	void MainPage::OnGlobalToggleChecked(const IInspectable&, const wux::RoutedEventArgs&) {
		AppSettings().TrackingEnabled = true;
		Lifetime::Current().TrackHotCorners();
	}
	void MainPage::OnGlobalToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&) {
		AppSettings().TrackingEnabled = false;
		Lifetime::Current().StopTracking();
	}

	void MainPage::OnTrayIconToggleChecked(const IInspectable&, const wux::RoutedEventArgs&) {
		AppSettings().TrayIconEnabled = true;
		Lifetime::Current().ShowTrayIcon();
	}
	void MainPage::OnTrayIconToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&) {
		AppSettings().TrayIconEnabled = false;
		Lifetime::Current().HideTrayIcon();
	}

	void MainPage::OnSettingAdded(const hstring& monitorId, const hstring& monitorName) {
		m_watcher.ConnectedDevices().Append({ monitorId, monitorName });
		MonitorPicker().SelectedIndex(m_watcher.ConnectedDevices().Size() - 1);

		AddConfigFlyout().RestartWatcher();
	}

	void MainPage::OnSettingRemoved(const hstring& monitorId) {
		if (const auto index = m_watcher.TryGetDeviceIndex(monitorId)) {
			MonitorPicker().SelectedIndex(*index - 1);
			m_watcher.ConnectedDevices().RemoveAt(*index);

			AddConfigFlyout().RestartWatcher();
		}
	}

	void MainPage::Save() const {
		AppSettings().Save();
		if (Lifetime::Started()) {
			Lifetime::Current().ReloadSettings();
		}
	}

	void MainPage::OnApplyButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Save();
	}

	winrt::fire_and_forget MainPage::OnOKButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Save();
		co_await wuvm::ApplicationView::GetForCurrentView().TryConsolidateAsync();
	}

	winrt::fire_and_forget MainPage::OnCancelButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		co_await wuvm::ApplicationView::GetForCurrentView().TryConsolidateAsync();
	}

	void MainPage::OnMonitorSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs&) {
		if (const auto monitor = MonitorPicker().SelectedItem().try_as<MonitorInfo>()) {
			SettingsView().SetMonitorId(monitor.Id());
		}
	}
}
