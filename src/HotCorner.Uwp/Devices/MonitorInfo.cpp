#include "pch.h"
#include "MonitorInfo.h"
#include "Devices/MonitorInfo.g.cpp"

namespace winrt::HotCorner::Uwp::Devices::implementation {
	MonitorInfo::MonitorInfo(const hstring& id, const hstring& name, bool connected) noexcept :
		m_id(id), m_name(name), m_connected(connected) { }
}