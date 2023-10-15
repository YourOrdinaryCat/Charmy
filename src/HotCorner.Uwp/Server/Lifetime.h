#pragma once
#include <winrt/HotCorner.Server.h>

namespace winrt::HotCorner::Uwp::Lifetime {
	const Server::LifetimeManager& Current() noexcept;
}
