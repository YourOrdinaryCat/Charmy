#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"
#include <App.h>
#include <Server/Lifetime.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;

using winrt::HotCorner::Uwp::Devices::MonitorInfo;
using App = winrt::HotCorner::Uwp::implementation::App;

namespace winrt::HotCorner::Uwp::Views::implementation {
	MainPage::MainPage() {

	}

	void MainPage::InitializeComponent() {
		MainPageT::InitializeComponent();

		MonitorPicker().ItemsSource(m_watcher.ConnectedDevices());
		m_watcher.Start();
	}

	void MainPage::OnStartStopButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().TrackHotCorners();
		Lifetime::Current().ShowTrayIcon();
	}

	static void Save() {
		App::Settings().Save();
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
			SettingsView().Monitor(monitor);
		}
	}
}
