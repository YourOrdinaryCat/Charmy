#pragma once
#include <Utils/Xaml.h>

#include "Views/NoMonitorsFlyout.g.h"

namespace winrt::HotCorner::Uwp::Views::implementation {
	struct NoMonitorsFlyout : NoMonitorsFlyoutT<NoMonitorsFlyout> {
		NoMonitorsFlyout() { }
	};
}

namespace winrt::HotCorner::Uwp::Views::factory_implementation {
	struct NoMonitorsFlyout : NoMonitorsFlyoutT<NoMonitorsFlyout, implementation::NoMonitorsFlyout> {
	};
}
