#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"

#include <Server/Lifetime.h>

namespace wdd = winrt::Windows::Devices::Display;
namespace wde = winrt::Windows::Devices::Enumeration;
namespace ws = winrt::Windows::System;
namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;

using winrt::HotCorner::Uwp::Devices::MonitorInfo;

namespace winrt::HotCorner::Uwp::Views::implementation {
	MainPage::MainPage() :
		m_watcher(wde::DeviceInformation::CreateWatcher(
			wdd::DisplayMonitor::GetDeviceSelector()
		)),
		m_addToken(m_watcher.Added({ this, &MainPage::OnDeviceAdded })),
		m_removeToken(m_watcher.Removed({ this, &MainPage::OnDeviceRemoved })),
		m_updateToken(m_watcher.Updated({ this, &MainPage::OnDeviceUpdated })),
		m_stoppedToken(m_watcher.Stopped({ this, &MainPage::OnDeviceEnumerationStopped })),
		m_monitors(winrt::single_threaded_observable_vector<MonitorInfo>()),
		DispatcherQueue(ws::DispatcherQueue::GetForCurrentThread())
	{
		m_watcher.Start();
	}

	MainPage::~MainPage() noexcept {
		m_watcher.Added(m_addToken);
		m_watcher.Removed(m_removeToken);
		m_watcher.Updated(m_updateToken);
		m_watcher.Stopped(m_stoppedToken);
	}

	void MainPage::InitializeComponent() {
		MainPageT::InitializeComponent();
		MonitorPicker().ItemsSource(m_monitors);
	}

	void MainPage::OnStartStopButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().TrackHotCorners();
		Lifetime::Current().ShowTrayIcon();
	}

	winrt::fire_and_forget MainPage::OnOKButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().HideTrayIcon();
		Lifetime::Current().StopTracking();

		co_await wuvm::ApplicationView::GetForCurrentView().TryConsolidateAsync();
	}

	winrt::fire_and_forget MainPage::OnCancelButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().HideTrayIcon();
		Lifetime::Current().StopTracking();

		co_await wuvm::ApplicationView::GetForCurrentView().TryConsolidateAsync();
	}

	void MainPage::OnMonitorSelected(const IInspectable&, const wuxc::SelectionChangedEventArgs& args) {
		const auto added = args.AddedItems().GetAt(0);
		if (const auto monitor = added.try_as<MonitorInfo>()) {
			SettingsView().Monitor(monitor);
		}
	}

	std::optional<uint32_t> MainPage::TryGetMonitorIndex(const hstring& id) const noexcept {
		const auto device = std::find_if(m_monitors.begin(), m_monitors.end(), [&id](const MonitorInfo& monitor)
			{
				return monitor.Id() == id;
			}
		);

		if (device == m_monitors.end()) {
			return std::nullopt;
		}

		return static_cast<uint32_t>(std::distance(m_monitors.begin(), device));
	}

	hstring GetMonitorName(const wdd::DisplayMonitor& monitor) {
		const hstring name = monitor.DisplayName();
		if (!name.empty()) {
			return name;
		}

		//TODO: Localize this, and maybe have better fallbacks?
		switch (monitor.ConnectionKind()) {
		case wdd::DisplayMonitorConnectionKind::Internal:
			return L"Internal Display";

		case wdd::DisplayMonitorConnectionKind::Virtual:
			return L"Virtual Display";

		case wdd::DisplayMonitorConnectionKind::Wireless:
			return L"Wirelesss Display";

		case wdd::DisplayMonitorConnectionKind::Wired:
			switch (monitor.PhysicalConnector()) {
			case wdd::DisplayMonitorPhysicalConnectorKind::AnalogTV:
				return L"Component Video Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::DisplayPort:
				return L"DisplayPort Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::Dvi:
				return L"DVI Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::HD15:
				return L"VGA Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::Hdmi:
				return L"HDMI Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::Lvds:
				return L"LVDS Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::Sdi:
				return L"SDI Display";
			}
		}

		return L"Unknown Display";
	}

	winrt::fire_and_forget MainPage::OnDeviceAdded(const wde::DeviceWatcher&, const wde::DeviceInformation& device) {
		const auto id = device.Id();
		const auto index = TryGetMonitorIndex(id);

		if (!index.has_value()) {
			const wdd::DisplayMonitor monitor =
				co_await wdd::DisplayMonitor::FromInterfaceIdAsync(id);

			const MonitorInfo info{ id, GetMonitorName(monitor), true};

			co_await wil::resume_foreground(DispatcherQueue);
			m_monitors.Append(info);
		}
		else {
			const auto monitor = m_monitors.GetAt(*index);

			co_await wil::resume_foreground(DispatcherQueue);
			monitor.Connected(true);
		}
	}

	winrt::fire_and_forget MainPage::OnDeviceRemoved(const wde::DeviceWatcher&, const wde::DeviceInformationUpdate& update) {
		const auto id = update.Id();

		if (const auto index = TryGetMonitorIndex(id)) {
			const auto info = m_monitors.GetAt(*index);

			co_await wil::resume_foreground(DispatcherQueue);
			info.Connected(false);
		}
	}

	winrt::fire_and_forget MainPage::OnDeviceUpdated(const wde::DeviceWatcher&, const wde::DeviceInformationUpdate& update) {
		const auto id = update.Id();

		if (const auto index = TryGetMonitorIndex(id)) {
			const wdd::DisplayMonitor monitor =
				co_await wdd::DisplayMonitor::FromInterfaceIdAsync(id);
			const auto info = m_monitors.GetAt(*index);

			co_await wil::resume_foreground(DispatcherQueue);
			info.DisplayName(GetMonitorName(monitor));
			info.Connected(true);
		}
	}

	void MainPage::OnDeviceEnumerationStopped(const wde::DeviceWatcher&, const IInspectable&) {
		if (m_watcher.Status() != wde::DeviceWatcherStatus::Stopped) {
			OutputDebugString(L"Device enumeration has stopped unexpectedly\n");
		}
	}
}
