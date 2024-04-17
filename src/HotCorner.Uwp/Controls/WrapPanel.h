#pragma once
#include <limits>
#include <Utils/Xaml.h>

#include "Controls/WrapPanel.g.h"

namespace winrt::HotCorner::Uwp::Controls::implementation {
	struct UIElementGroup {
		float Length;
		float Extent;

		std::vector<Windows::Foundation::Rect> Children;

		constexpr UIElementGroup(float length, float extent) noexcept :
			Length(length), Extent(extent) { }
	};

	struct WrapPanel : WrapPanelT<WrapPanel> {
	private:
		static void OnLayoutPropertyChanged(
			const wux::DependencyObject& sender,
			const wux::DependencyPropertyChangedEventArgs&)
		{
			if (const auto elm = sender.try_as<class_type>()) {
				elm.InvalidateMeasure();
				elm.InvalidateArrange();
			}
		}

	public:
		WrapPanel() { }

		Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size);
		Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size);

		DEPENDENCY_PROPERTY_META(
			Orientation,
			wuxc::Orientation,
			box_value(wuxc::Orientation::Horizontal),
			OnLayoutPropertyChanged
		);

		DEPENDENCY_PROPERTY_META(
			ItemWidth,
			double,
			box_value(std::numeric_limits<double>::quiet_NaN()),
			OnLayoutPropertyChanged
		);

		DEPENDENCY_PROPERTY_META(
			ItemHeight,
			double,
			box_value(std::numeric_limits<double>::quiet_NaN()),
			OnLayoutPropertyChanged
		);

		DEPENDENCY_PROPERTY_META(
			HorizontalSpacing,
			double,
			box_value(0.0),
			OnLayoutPropertyChanged
		);

		DEPENDENCY_PROPERTY_META(
			VerticalSpacing,
			double,
			box_value(0.0),
			OnLayoutPropertyChanged
		);

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
