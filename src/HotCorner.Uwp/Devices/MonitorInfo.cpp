#include "pch.h"
#include "MonitorInfo.h"
#include "Devices/MonitorInfo.g.cpp"

#include <Localization.h>
#include <winrt/Windows.Devices.Display.h>

namespace wdd = winrt::Windows::Devices::Display;
namespace wde = winrt::Windows::Devices::Enumeration;
namespace wf = winrt::Windows::Foundation;

namespace winrt::HotCorner::Uwp::Devices::implementation {
	static wf::IAsyncOperation<hstring> GetMonitorNameAsync(const hstring& id) {
		const wdd::DisplayMonitor monitor{
			co_await wdd::DisplayMonitor::FromInterfaceIdAsync(id)
		};

		const hstring name = monitor.DisplayName();
		if (!name.empty()) {
			co_return name;
		}

		using ConnectionKind = wdd::DisplayMonitorConnectionKind;
		using PhysicalConnector = wdd::DisplayMonitorPhysicalConnectorKind;

		switch (monitor.ConnectionKind()) {
		case ConnectionKind::Internal:
			co_return StringLoader().GetString(L"ConnectionKindInternal");

		case ConnectionKind::Virtual:
			co_return StringLoader().GetString(L"ConnectionKindVirtual");

		case ConnectionKind::Wireless:
			co_return StringLoader().GetString(L"ConnectionKindWireless");

		case ConnectionKind::Wired:
			switch (monitor.PhysicalConnector()) {
			case PhysicalConnector::AnalogTV:
				co_return StringLoader().GetString(L"PhysicalConnectorAnalogTV");

			case PhysicalConnector::DisplayPort:
				co_return StringLoader().GetString(L"PhysicalConnectorDisplayPort");

			case PhysicalConnector::Dvi:
				co_return StringLoader().GetString(L"PhysicalConnectorDvi");

			case PhysicalConnector::HD15:
				co_return StringLoader().GetString(L"PhysicalConnectorHD15");

			case PhysicalConnector::Hdmi:
				co_return StringLoader().GetString(L"PhysicalConnectorHdmi");

			case PhysicalConnector::Lvds:
				co_return StringLoader().GetString(L"PhysicalConnectorLvds");

			case PhysicalConnector::Sdi:
				co_return StringLoader().GetString(L"PhysicalConnectorSdi");
			}
		}

		co_return StringLoader().GetString(L"GenericDisplay");
	}

	wf::IAsyncAction MonitorInfo::RefreshAsync(const hstring& id) {
		DisplayName(co_await GetMonitorNameAsync(id));
	}

	MonitorInfo::MonitorInfo(const hstring& id, const hstring& name) noexcept :
		m_id(id), m_name(name) { }

	MonitorInfo::MonitorInfo(const wde::DeviceInformation& device) :
		m_id(device.Id()), m_name(GetMonitorNameAsync(m_id).get()) { }
}
