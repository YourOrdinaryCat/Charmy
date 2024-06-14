#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"

#include <Localization.h>
#include <Server/Lifetime.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace wamdt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wuvm = winrt::Windows::UI::ViewManagement;

using winrt::HotCorner::Uwp::Devices::MonitorInfo;

namespace winrt::HotCorner::Uwp::Views::implementation {
	void MainPage::InitializeComponent() {
		MainPageT::InitializeComponent();

		const auto connected = m_watcher.ConnectedDevices();

		connected.Append({ L"", StringLoader().GetString(L"DefaultSettings") });
		for (auto&& setting : AppSettings().Monitors) {
			connected.Append({ setting.first, setting.second.DisplayName });
		}

		MonitorPicker().ItemsSource(connected);
		m_watcher.Start();
	}

	static void SwitchTracking(bool track) {
		if (track) {
			Lifetime::Current().TrackHotCorners();
		}
		else {
			Lifetime::Current().StopTracking();
		}
	}

	static void SwitchTrayIcon(bool show) {
		if (show) {
			Lifetime::Current().ShowTrayIcon();
		}
		else {
			Lifetime::Current().HideTrayIcon();
		}
	}

	static void OnGlobalCheckUpdated(bool checked) {
		AppSettings().TrackingEnabled = checked;
		SwitchTracking(checked);
	}

	static void OnTrayIconCheckUpdated(bool checked) {
		AppSettings().TrayIconEnabled = checked;
		SwitchTrayIcon(checked);
	}

	winrt::fire_and_forget MainPage::OnPageLoaded(const IInspectable&, const wux::RoutedEventArgs&) {
		const bool track = AppSettings().TrackingEnabled;
		const bool show = AppSettings().TrayIconEnabled;

		const auto gc = GlobalCheck();
		const auto tic = TrayIconCheck();

		gc.IsChecked(track);
		tic.IsChecked(show);

		gc.Checked([](const IInspectable&, const wux::RoutedEventArgs&)
			{ OnGlobalCheckUpdated(true); }
		);
		gc.Unchecked([](const IInspectable&, const wux::RoutedEventArgs&)
			{ OnGlobalCheckUpdated(false); }
		);

		tic.Checked([](const IInspectable&, const wux::RoutedEventArgs&)
			{ OnTrayIconCheckUpdated(true); }
		);
		tic.Unchecked([](const IInspectable&, const wux::RoutedEventArgs&)
			{ OnTrayIconCheckUpdated(false); }
		);

		// Start up the server - it will automatically start/stop tracking according to
		// user's settings
		co_await winrt::resume_background();
		std::ignore = Lifetime::Current();
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
		Lifetime::Current().ReloadSettings();
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
			MonitorPicker().PlaceholderText(monitor.DisplayName());
		}
	}

	void MainPage::OnCopyVersionClick(const IInspectable&, const wux::RoutedEventArgs&) {
		const wamdt::DataPackage dp{};
		dp.RequestedOperation(wamdt::DataPackageOperation::Copy);
		dp.SetText(L"Charmy - UAP v1.0.0");

		wamdt::Clipboard::SetContent(dp);
	}
}
