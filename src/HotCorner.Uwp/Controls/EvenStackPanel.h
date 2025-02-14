#pragma once
#include <Utils/Xaml.h>

#include "Controls/EvenStackPanel.g.h"

namespace winrt::HotCorner::Uwp::Controls::implementation {
	/**
	 * @brief Arranges child elements into a single line that can be oriented
	 *        horizontally or vertically, giving each element the same size.
	*/
	struct EvenStackPanel : EvenStackPanelT<EvenStackPanel> {
		EvenStackPanel() {}

		Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size);
		Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size) const;

		static void EnsureProperties();

		DEPENDENCY_PROPERTY_API(Orientation, wuxc::Orientation);
		DEPENDENCY_PROPERTY_API(Spacing, double);

	private:
		Windows::Foundation::Size m_maxSize{};
	};
}

namespace winrt::HotCorner::Uwp::Controls::factory_implementation {
	struct EvenStackPanel : EvenStackPanelT<EvenStackPanel, implementation::EvenStackPanel> {
	};
}
