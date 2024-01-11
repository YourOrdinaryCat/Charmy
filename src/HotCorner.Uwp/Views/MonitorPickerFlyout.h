#pragma once
#include <AppSettings.h>
#include <Devices/MonitorInfo.h>
#include <Devices/Watcher.h>
#include <Utils/Xaml.h>
#include <winrt/Windows.Devices.Display.h>

#include "Views/MonitorPickerFlyout.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	using namespace XamlAliases;

	struct MonitorPickerFlyoutWatcher final : public Devices::Watcher<Devices::MonitorInfo> {
		MonitorPickerFlyoutWatcher() :
			Devices::Watcher<Devices::MonitorInfo>(Windows::Devices::Display::DisplayMonitor::GetDeviceSelector()) { }

	protected:
		bool AddingDeviceOverride(const Devices::MonitorInfo& info) const override {
			return !AppSettings().Monitors.contains(info.Id());
		}
	};

	struct MonitorPickerFlyout : MonitorPickerFlyoutT<MonitorPickerFlyout> {
		using WatcherT = MonitorPickerFlyoutWatcher;

		MonitorPickerFlyout() { }

		void InitializeComponent();

		event_token SettingAdded(const SettingAddedEventHandler& handler);
		void SettingAdded(const event_token& token) noexcept;

		void RestartWatcher();

		void OnMonitorListItemClick(const IInspectable&, const wuxc::ItemClickEventArgs&);

	private:
		event<SettingAddedEventHandler> m_settingAddedEvent;

		WatcherT m_watcher{};

		void OnConnectedDevicesChanged(const WatcherT::DeviceVectorT&, const WatcherT::DeviceVectorChangedArgsT&);
		const event_token m_devicesChangedToken = m_watcher.ConnectedDevices().
			VectorChanged({ this, &MonitorPickerFlyout::OnConnectedDevicesChanged });
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MonitorPickerFlyout : MonitorPickerFlyoutT<MonitorPickerFlyout, implementation::MonitorPickerFlyout> {
	};
}
