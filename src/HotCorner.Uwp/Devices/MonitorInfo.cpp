#include "pch.h"
#include "MonitorInfo.h"
#include "Devices/MonitorInfo.g.cpp"

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

		//TODO: Localize this, and maybe have better fallbacks?
		switch (monitor.ConnectionKind()) {
		case wdd::DisplayMonitorConnectionKind::Internal:
			co_return L"Internal Display";

		case wdd::DisplayMonitorConnectionKind::Virtual:
			co_return L"Virtual Display";

		case wdd::DisplayMonitorConnectionKind::Wireless:
			co_return L"Wirelesss Display";

		case wdd::DisplayMonitorConnectionKind::Wired:
			switch (monitor.PhysicalConnector()) {
			case wdd::DisplayMonitorPhysicalConnectorKind::AnalogTV:
				co_return L"Component Video Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::DisplayPort:
				co_return L"DisplayPort Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::Dvi:
				co_return L"DVI Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::HD15:
				co_return L"VGA Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::Hdmi:
				co_return L"HDMI Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::Lvds:
				co_return L"LVDS Display";

			case wdd::DisplayMonitorPhysicalConnectorKind::Sdi:
				co_return L"SDI Display";
			}
		}

		co_return L"Unknown Display";
	}

	wf::IAsyncAction MonitorInfo::RefreshAsync(const hstring& id) {
		DisplayName(co_await GetMonitorNameAsync(id));
	}

	MonitorInfo::MonitorInfo(const hstring& id, const hstring& name) noexcept :
		m_id(id), m_name(name) { }

	MonitorInfo::MonitorInfo(const wde::DeviceInformation& device) :
		m_id(device.Id()), m_name(GetMonitorNameAsync(m_id).get()) { }
}
