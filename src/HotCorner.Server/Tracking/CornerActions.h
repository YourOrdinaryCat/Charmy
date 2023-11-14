#pragma once
#include <CornerAction.h>
#include <functional>
#include <optional>

namespace winrt::HotCorner::Server::Tracking {
	std::optional<std::function<bool()>> GetDelegate(Settings::CornerAction action) noexcept;

	bool OpenTaskView() noexcept;
	bool OpenStart() noexcept;
	bool OpenSearch() noexcept;
	bool ToggleDesktop() noexcept;
}
