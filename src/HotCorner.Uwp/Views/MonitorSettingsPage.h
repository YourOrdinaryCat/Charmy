#pragma once
#include <Devices/MonitorInfo.h>
#include <Utils/Property.h>

#include "Views/MonitorSettingsPage.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	/**
	 * @brief Shown to the user when a monitor is picked.
	*/
	struct MonitorSettingsPage : MonitorSettingsPageT<MonitorSettingsPage> {
		MonitorSettingsPage() {

		}

		DEPENDENCY_PROPERTY(Monitor, Devices::MonitorInfo);
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct MonitorSettingsPage : MonitorSettingsPageT<MonitorSettingsPage, implementation::MonitorSettingsPage> {
	};
}
