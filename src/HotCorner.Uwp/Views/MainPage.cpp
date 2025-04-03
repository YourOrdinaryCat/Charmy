#include "pch.h"

#include "MainPage.h"
#include "Views/MainPage.g.cpp"

#include "NoMonitorsFlyout.h"
#include <Localization.h>
#include <Logging.h>
#include <Server/Lifetime.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Input.h>

namespace wamdt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wdd = winrt::Windows::Devices::Display;
namespace wde = winrt::Windows::Devices::Enumeration;
namespace wf = winrt::Windows::Foundation;
namespace wst = winrt::Windows::Storage;
namespace wsys = winrt::Windows::System;
namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;

using winrt::HotCorner::Uwp::Devices::MonitorInfo;

namespace winrt::HotCorner::Uwp::Views::implementation {
	winrt::fire_and_forget MainPage::InitializeComponent() {
		MainPageT::InitializeComponent();

		const auto connected = m_watcher.ConnectedDevices();

		connected.Append({ L"", StringLoader().GetString(L"DefaultSettings") });
		for (auto&& setting : AppSettings().Monitors) {
			connected.Append({ setting.first, setting.second.DisplayName });
		}

		MonitorPicker().ItemsSource(connected);
		m_watcher.Start();

		co_await winrt::resume_background();
		if (AppSettings().TrackingEnabled) {
			co_await Lifetime::Current().BeginTrackingAsync();
		}

		if (AppSettings().TrayIconEnabled) {
			Lifetime::Current().ShowTrayIcon();
		}
	}

	void MainPage::OnGlobalCheckLoaded(const IInspectable& sender, const wux::RoutedEventArgs&) {
		sender.as<wuxc::CheckBox>().IsChecked(AppSettings().TrackingEnabled);
	}

	void MainPage::OnTrayIconCheckLoaded(const IInspectable& sender, const wux::RoutedEventArgs&) {
		sender.as<wuxc::CheckBox>().IsChecked(AppSettings().TrayIconEnabled);
	}

	winrt::fire_and_forget MainPage::OnGlobalCheckClick(const IInspectable& sender, const wux::RoutedEventArgs&) {
		const bool checked = sender.as<wuxc::CheckBox>().IsChecked().GetBoolean();
		AppSettings().TrackingEnabled = checked;

		if (checked) {
			co_await Lifetime::Current().BeginTrackingAsync();
		}
		else {
			co_await Lifetime::Current().StopTrackingAsync();
		}
	}

	void MainPage::OnTrayIconCheckClick(const IInspectable& sender, const wux::RoutedEventArgs&) {
		const bool checked = sender.as<wuxc::CheckBox>().IsChecked().GetBoolean();
		AppSettings().TrayIconEnabled = checked;

		if (checked) {
			Lifetime::Current().ShowTrayIcon();
		}
		else {
			Lifetime::Current().HideTrayIcon();
		}
	}

	winrt::fire_and_forget MainPage::OnAddButtonClick(const IInspectable& sender, const wux::RoutedEventArgs&) {
		const auto btn = sender.as<wuxc::Button>();
		const wde::DeviceInformationCollection devices{
			co_await wde::DeviceInformation::FindAllAsync(wdd::DisplayMonitor::GetDeviceSelector())
		};

		const wuxc::MenuFlyout mf{};
		for (auto&& info : devices) {
			if (!AppSettings().Monitors.contains(info.Id())) {
				const Devices::MonitorInfo monitor{
					co_await Devices::MonitorInfo::FromDeviceAsync(info)
				};

				const wuxc::MenuFlyoutItem mfi{};
				mfi.Text(monitor.DisplayName());
				mfi.Tag(monitor);
				mfi.Click({ this, &MainPage::OnMonitorClick });
				mf.Items().Append(mfi);
			}
		}

		if (mf.Items().Size() != 0) {
			const wuxcp::FlyoutShowOptions opts{};
			opts.Placement(wuxcp::FlyoutPlacementMode::Bottom);
			mf.ShowAt(btn, opts);
		}
		else {
			const Views::NoMonitorsFlyout nmf{};
			nmf.ShowAt(btn);
		}
	}

	void MainPage::OnMonitorClick(const IInspectable& sender, const wux::RoutedEventArgs&) {
		const auto monitor = sender.as<wuxc::MenuFlyoutItem>().Tag().as<MonitorInfo>();

		// Copy default settings and update the name
		Settings::MonitorSettings ns = AppSettings().DefaultSettings;
		ns.DisplayName = monitor.DisplayName();

		AppSettings().Monitors.insert({ std::wstring{ monitor.Id() }, ns });

		m_watcher.ConnectedDevices().Append(monitor);
		MonitorPicker().SelectedIndex(m_watcher.ConnectedDevices().Size() - 1);
	}

	void MainPage::OnSettingRemoved(const hstring& monitorId) {
		if (const auto index = m_watcher.TryGetDeviceIndex(monitorId)) {
			MonitorPicker().SelectedIndex(*index - 1);
			m_watcher.ConnectedDevices().RemoveAt(*index);
		}
	}

	void MainPage::Save() const {
		AppSettings().Save(SettingsPath().c_str());
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
		dp.SetText(L"Charmy - UAP v1.1.0");

		wamdt::Clipboard::SetContent(dp);
	}

	winrt::fire_and_forget MainPage::OnOpenLogFolderClick(const IInspectable&, const wux::RoutedEventArgs&) {
		using enum Logging::lazy_sink_state;
		const auto sink = Logging::FileSink();

		if (const auto sink = Logging::FileSink()) {
			const wsys::FolderLauncherOptions opt{};
			const auto folder = SettingsFolder();

			if (sink->state() == opened) {
				const auto file = sink->file().filename();
				opt.ItemsToSelect().Append(co_await folder.TryGetItemAsync(file.c_str()));
			}

			if (!(co_await wsys::Launcher::LaunchFolderAsync(folder, opt))) {
				spdlog::error("Failed to open log folder");
			}
		}
	}

	void MainPage::OnLogVerbosityFlyoutOpening(const IInspectable& sender, const IInspectable&) {
		const auto currLevel = AppSettings().LogVerbosity;
		for (auto&& item : sender.as<wuxc::MenuFlyout>().Items()) {
			if (const auto rmfi = item.try_as<muxc::RadioMenuFlyoutItem>()) {
				if (rmfi.Tag().as<int>() == currLevel) {
					rmfi.IsChecked(true);
					break;
				}
			}
		}
	}

	void MainPage::OnLogLevelClick(const IInspectable& sender, const wux::RoutedEventArgs&) {
		AppSettings().LogVerbosity = static_cast<spdlog::level::level_enum>
			(sender.as<muxc::RadioMenuFlyoutItem>().Tag().as<int>());
	}
}
