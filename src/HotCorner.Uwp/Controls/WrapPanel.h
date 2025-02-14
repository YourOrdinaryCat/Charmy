#pragma once
#include <limits>
#include <Utils/Xaml.h>

#include "Controls/WrapPanel.g.h"
#include <winrt/Windows.Foundation.h>

namespace winrt::HotCorner::Uwp::Controls::implementation {
	struct UIElementGroup {
		float Length;
		float Extent;

		std::vector<Windows::Foundation::Rect> Children;

		constexpr UIElementGroup(float length, float extent) noexcept :
			Length(length), Extent(extent)
		{
		}
	};

	/**
	 * @brief Positions child elements sequentially from left to right or top to bottom.
	 *        When elements extend beyond the container edge, they are positioned in the
	 *        next row or column. Based on the Windows Community Toolkit control with
	 *        the same name.
	*/
	struct WrapPanel : WrapPanelT<WrapPanel> {
		WrapPanel() {}

		Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size);
		Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size);

		static void EnsureProperties();

		DEPENDENCY_PROPERTY_API(Orientation, wuxc::Orientation);
		DEPENDENCY_PROPERTY_API(ItemWidth, double);
		DEPENDENCY_PROPERTY_API(ItemHeight, double);
		DEPENDENCY_PROPERTY_API(HorizontalSpacing, double);
		DEPENDENCY_PROPERTY_API(VerticalSpacing, double);

	private:
		std::vector<UIElementGroup> m_groups;
		Windows::Foundation::Size UpdateGroups(
			Windows::Foundation::Size constraint,
			bool isHorizontal,
			bool shouldMeasure
		);
	};
}

namespace winrt::HotCorner::Uwp::Controls::factory_implementation {
	struct WrapPanel : WrapPanelT<WrapPanel, implementation::WrapPanel> {
	};
}
