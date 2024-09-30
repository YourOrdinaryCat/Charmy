#include "pch.h"
#include "WrapPanel.h"

#include "Controls/WrapPanel.g.cpp"

namespace wf = winrt::Windows::Foundation;

namespace winrt::HotCorner::Uwp::Controls::implementation {
	static constexpr float GetLength(bool isHorizontal, wf::Size size) noexcept {
		return isHorizontal ? size.Width : size.Height;
	}

	static constexpr float GetExtent(bool isHorizontal, wf::Size size) noexcept {
		return isHorizontal ? size.Height : size.Width;
	}

	static constexpr wf::Size ToSize(
		bool isHorizontal,
		float length,
		float extent) noexcept
	{
		return isHorizontal ? wf::Size{ length, extent } : wf::Size{ extent, length };
	}

	static constexpr wf::Point ToPoint(
		bool isHorizontal,
		float length,
		float extent) noexcept
	{
		return isHorizontal ? wf::Point{ length, extent } : wf::Point{ extent, length };
	}

	wf::Size WrapPanel::MeasureOverride(wf::Size availableSize) {
		return UpdateGroups(
			availableSize,
			Orientation() == wuxc::Orientation::Horizontal,
			true
		);
	}

	wf::Size WrapPanel::ArrangeOverride(wf::Size finalSize) {
		const bool isHorizontal = Orientation() == wuxc::Orientation::Horizontal;
		const auto desiredSize = DesiredSize();

		if ((isHorizontal && finalSize.Width < desiredSize.Width) ||
			(!isHorizontal && finalSize.Height < desiredSize.Height))
		{
			UpdateGroups(finalSize, isHorizontal, false);
		}

		const auto children = Children();
		uint32_t i = 0;

		for (auto&& group : m_groups) {
			for (auto&& rect : group.Children) {
				auto child = children.GetAt(i);

				while (child.Visibility() != wux::Visibility::Visible) {
					child = children.GetAt(++i);
				}

				child.Arrange({
					rect.X,
					rect.Y,
					isHorizontal ? rect.Width : group.Extent,
					isHorizontal ? group.Extent : rect.Height
				});
				++i;
			}
		}

		return finalSize;
	}

	wf::Size WrapPanel::UpdateGroups(
		wf::Size availableSize,
		bool isHorizontal,
		bool shouldMeasure)
	{
		const float iw = static_cast<float>(ItemWidth());
		const float ih = static_cast<float>(ItemHeight());
		const bool hasIw = !isnan(iw);
		const bool hasIh = !isnan(ih);

		const wf::Size childrenConstraint{
			hasIw ? iw : availableSize.Width,
			hasIh ? ih : availableSize.Height
		};

		// Here starts the use of orientation independent terminology - this allows the
		// same code path to be used for horizontal and vertical orientations
		const bool useChildLength = !(isHorizontal ? hasIw : hasIh);
		const bool useChildExtent = !(isHorizontal ? hasIh : hasIw);

		const float lengthSpacing = static_cast<float>(
			isHorizontal ? HorizontalSpacing() : VerticalSpacing());

		const float extentSpacing = static_cast<float>(
			isHorizontal ? VerticalSpacing() : HorizontalSpacing());

		const float maxLength = GetLength(isHorizontal, availableSize);

		float desiredLength = 0;
		float desiredExtent = 0;

		m_groups.clear();
		UIElementGroup currGroup{ 0,0 };

		for (auto&& child : Children()) {
			if (shouldMeasure) {
				child.Measure(childrenConstraint);
			}

			// If requested, we still want to measure non visible children - however,
			// they should be skipped from this point on
			if (child.Visibility() != wux::Visibility::Visible) {
				continue;
			}

			const auto childDesired = child.DesiredSize();
			const float childLength = GetLength(
				isHorizontal,
				useChildLength ? childDesired : childrenConstraint
			);
			const float childExtent = GetExtent(
				isHorizontal,
				useChildExtent ? childDesired : childrenConstraint
			);

			// Start a new group if necessary - note that a new group is never started
			// when the current one has no children, because that would force an extra
			// group if the child overflows
			if (!currGroup.Children.empty() &&
				currGroup.Length + childLength > maxLength)
			{
				// Remove the trailing spacing leftover from the previous child
				currGroup.Length -= lengthSpacing;

				// Update our desired measurements now that every child is accounted for
				desiredLength = std::max(desiredLength, currGroup.Length);
				desiredExtent += currGroup.Extent + extentSpacing;

				m_groups.push_back(currGroup);
				currGroup = { 0,0 };
			}

			const wf::Rect slot{
				ToPoint(isHorizontal, currGroup.Length, desiredExtent),
				ToSize(isHorizontal, childLength, childExtent)
			};

			currGroup.Children.push_back(slot);
			currGroup.Length += childLength + lengthSpacing;
			currGroup.Extent = std::max(currGroup.Extent, childExtent);
		}

		// Remove last group if empty - otherwise, adjust measurements for the most
		// recently added group
		if (currGroup.Children.empty()) {
			if (!m_groups.empty()) {
				m_groups.pop_back();
				desiredExtent -= extentSpacing;
			}
		}
		else {
			currGroup.Length -= lengthSpacing;
			desiredLength = std::max(desiredLength, currGroup.Length);
			desiredExtent += currGroup.Extent;

			m_groups.push_back(currGroup);
		}

		return ToSize(isHorizontal, desiredLength, desiredExtent);
	}
}
