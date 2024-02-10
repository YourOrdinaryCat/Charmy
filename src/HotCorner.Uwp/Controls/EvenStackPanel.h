#pragma once
#include <Utils/Property.h>
#include <Utils/Xaml.h>

#include "Controls/EvenStackPanel.g.h"

namespace winrt::HotCorner::Uwp::Controls::implementation {
	using namespace XamlAliases;

	/**
	 * @brief Shown to the user when the app is launched normally
	 *        or through the tray icon.
	*/
	struct EvenStackPanel : EvenStackPanelT<EvenStackPanel> {
		EvenStackPanel() { }

		Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size);
		Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size) const;

		DEPENDENCY_PROPERTY_META(Orientation, wuxc::Orientation, box_value(wuxc::Orientation::Vertical), nullptr);
		DEPENDENCY_PROPERTY_META(Spacing, double, box_value(0.0), nullptr);

	private:
		Windows::Foundation::Size m_maxSize{};
	};
}

namespace winrt::HotCorner::Uwp::Controls::factory_implementation {
	struct EvenStackPanel : EvenStackPanelT<EvenStackPanel, implementation::EvenStackPanel> {
	};
}
