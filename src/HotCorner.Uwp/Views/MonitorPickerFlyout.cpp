#include "pch.h"
#include "MonitorPickerFlyout.h"
#include "Views/MonitorPickerFlyout.g.cpp"

namespace wfc = winrt::Windows::Foundation::Collections;

using winrt::HotCorner::Uwp::Devices::MonitorInfo;

namespace winrt::HotCorner::Uwp::Views::implementation {
	void MonitorPickerFlyout::InitializeComponent() {
		MonitorPickerFlyoutT::InitializeComponent();

		MonitorList().ItemsSource(m_watcher.ConnectedDevices());
		m_watcher.Start();
	}

	event_token MonitorPickerFlyout::SettingAdded(const SettingAddedEventHandler& handler) {
		return m_settingAddedEvent.add(handler);
	}
	void MonitorPickerFlyout::SettingAdded(const event_token& token) noexcept {
		return m_settingAddedEvent.remove(token);
	}

	void MonitorPickerFlyout::RestartWatcher() {
		m_watcher.Restart(true);
	}

	void MonitorPickerFlyout::OnMonitorListItemClick(const IInspectable&, const wuxc::ItemClickEventArgs& args) {
		if (const auto monitor = args.ClickedItem().try_as<MonitorInfo>()) {
			Settings::MonitorSettings ns = AppSettings().DefaultSettings;
			ns.DisplayName = monitor.DisplayName();

			AppSettings().Monitors.insert({ std::wstring{ monitor.Id() }, ns });

			m_settingAddedEvent(monitor.Id(), monitor.DisplayName());
			Hide();
		}
	}

	void MonitorPickerFlyout::OnConnectedDevicesChanged(const WatcherT::DeviceVectorT& sender, const WatcherT::DeviceVectorChangedArgsT&) {
		if (sender.Size() == 0) {
			MonitorList().Visibility(wux::Visibility::Collapsed);
			NoMonitorsInfo().IsOpen(true);
		}
		else {
			MonitorList().Visibility(wux::Visibility::Visible);
			NoMonitorsInfo().IsOpen(false);
		}
	}
}
