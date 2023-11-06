#include "pch.h"
#include "MonitorInfo.h"
#include "Devices/MonitorInfo.g.cpp"

#include <winrt/Windows.Devices.Display.h>

namespace wdd = winrt::Windows::Devices::Display;
namespace wde = winrt::Windows::Devices::Enumeration;

namespace winrt::HotCorner::Uwp::Devices::implementation {
	hstring GetMonitorName(const hstring& id) {
		const wdd::DisplayMonitor monitor{
			wdd::DisplayMonitor::FromInterfaceIdAsync(id).get()
		};

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

	void MonitorInfo::Refresh(const hstring& id) {
		DisplayName(GetMonitorName(id));
		Connected(true);
	}

	MonitorInfo::MonitorInfo(const hstring& id, const hstring& name, bool connected) noexcept :
		m_id(id), m_name(name), m_connected(connected) { }

	MonitorInfo::MonitorInfo(const wde::DeviceInformation& device) :
		m_id(device.Id()),
		m_name(GetMonitorName(m_id)),
		m_connected(true) { }
}