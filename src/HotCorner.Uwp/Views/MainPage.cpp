#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"
#include <Server/Lifetime.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;

using winrt::HotCorner::Uwp::Devices::MonitorInfo;

namespace winrt::HotCorner::Uwp::Views::implementation {
	void MainPage::InitializeComponent() {
		MainPageT::InitializeComponent();

		GlobalCheck().IsChecked(m_settings.TrackingEnabled);
		TrayIconCheck().IsChecked(m_settings.TrayIconEnabled);

		const auto connected = m_watcher.ConnectedDevices();

		connected.Append({ L"", L"Default settings" });
		for (const auto& setting : m_settings.Monitors) {
			connected.Append({ setting.first, setting.second.DisplayName });
		}

		MonitorPicker().ItemsSource(connected);
		m_watcher.Start();
	}

	void MainPage::OnGlobalToggleChecked(const IInspectable&, const wux::RoutedEventArgs&) {
		m_settings.TrackingEnabled = true;
		Lifetime::Current().TrackHotCorners();
	}
	void MainPage::OnGlobalToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&) {
		m_settings.TrackingEnabled = false;
		Lifetime::Current().StopTracking();
	}

	void MainPage::OnTrayIconToggleChecked(const IInspectable&, const wux::RoutedEventArgs&) {
		m_settings.TrayIconEnabled = true;
		Lifetime::Current().ShowTrayIcon();
	}
	void MainPage::OnTrayIconToggleUnchecked(const IInspectable&, const wux::RoutedEventArgs&) {
		m_settings.TrayIconEnabled = false;
		Lifetime::Current().HideTrayIcon();
	}

	void MainPage::Save() const {
		m_settings.Save();
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

	void MainPage::OnMonitorSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs& args) {
		const auto added = args.AddedItems().GetAt(0);
		if (const auto monitor = added.try_as<MonitorInfo>()) {
			SettingsView().SetMonitorId(monitor.Id());
		}
	}
}
