#include "pch.h"
#include "EvenStackPanel.h"
#include "Controls/EvenStackPanel.g.cpp"

namespace wf = winrt::Windows::Foundation;

namespace winrt::HotCorner::Uwp::Controls::implementation {
	wf::Size EvenStackPanel::MeasureOverride(wf::Size availableSize) {
		const auto orientation = Orientation();
		const bool isHorizontal = orientation == wuxc::Orientation::Horizontal;

		const auto constraint = isHorizontal ?
			wf::Size{ availableSize.Width, INFINITY } :
			wf::Size{ INFINITY, availableSize.Height };

		wf::Size desiredSize{};
		for (auto&& child : Children()) {
			child.Measure(constraint);

			desiredSize.Width = std::max(desiredSize.Width, child.DesiredSize().Width);
			desiredSize.Height = std::max(desiredSize.Height, child.DesiredSize().Height);
		}

		m_maxSize = desiredSize;

		const auto childrenCount = Children().Size();
		if (isHorizontal) {
			desiredSize.Width *= childrenCount;
		}
		else {
			desiredSize.Height *= childrenCount;
		}

		if (childrenCount > 1) {
			const auto combinedSpacing = static_cast<float>((childrenCount - 1) * Spacing());
			if (isHorizontal) {
				desiredSize.Width += combinedSpacing;
			}
			else {
				desiredSize.Height += combinedSpacing;
			}
		}

		return desiredSize;
	}

	wf::Size EvenStackPanel::ArrangeOverride(wf::Size finalSize) const {
		wf::Rect arrangeRect{
			0,
			0,
			m_maxSize.Width,
			m_maxSize.Height
		};

		for (auto&& child : Children()) {
			child.Arrange(arrangeRect);
			if (Orientation() == wuxc::Orientation::Horizontal) {
				arrangeRect.X += arrangeRect.Width + static_cast<float>(Spacing());
			}
			else {
				arrangeRect.Y += arrangeRect.Height + static_cast<float>(Spacing());
			}
		}

		return finalSize;
	}
}
