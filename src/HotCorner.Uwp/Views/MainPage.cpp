#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"

#include <Localization.h>
#include <Logging.h>
#include <Server/Lifetime.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace wamdt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wf = winrt::Windows::Foundation;
namespace wst = winrt::Windows::Storage;
namespace wsys = winrt::Windows::System;
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
		Logging::FileSink()->set_level(AppSettings().LogVerbosity);

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

	winrt::fire_and_forget MainPage::OnOpenLogFolderClick(const IInspectable&, const wux::RoutedEventArgs&) {
		using enum Logging::lazy_sink_state;
		const auto sink = Logging::FileSink();

		if (const auto sink = Logging::FileSink()) {
			const wsys::FolderLauncherOptions opt{};

			if (sink->state() == opened) {
				const auto file = sink->file().filename();
				opt.ItemsToSelect().Append(co_await SettingsFolder.TryGetItemAsync(file.c_str()));
			}

			if (!(co_await wsys::Launcher::LaunchFolderAsync(SettingsFolder, opt))) {
				spdlog::error("Failed to open log folder");
			}
		}
	}

	void MainPage::OnLogVerbosityFlyoutOpening(const IInspectable& sender, const IInspectable&) {
		const auto currLevel = AppSettings().LogVerbosity;
		for (auto&& item : sender.as<wuxc::MenuFlyout>().Items()) {
			if (const auto rmfi = item.try_as<muxc::RadioMenuFlyoutItem>()) {
				if (rmfi.Tag().as<int32_t>() == currLevel) {
					rmfi.IsChecked(true);
					break;
				}
			}
		}
	}

	void MainPage::OnLogLevelClick(const IInspectable& sender, const wux::RoutedEventArgs&) {
		AppSettings().LogVerbosity = static_cast<spdlog::level::level_enum>
			(sender.as<muxc::RadioMenuFlyoutItem>().Tag().as<int32_t>());
	}
}
