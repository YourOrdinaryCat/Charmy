#pragma once
#include <winrt/HotCorner.Server.h>

namespace winrt::HotCorner::Uwp::Lifetime {
	inline bool Started() noexcept;
	const Server::LifetimeManager& Current() noexcept;
}
