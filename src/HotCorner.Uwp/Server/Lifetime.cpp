#include "pch.h"
#include "Lifetime.h"

namespace winrt::HotCorner::Uwp::Lifetime {
	const Server::LifetimeManager& Current() noexcept {
		static const auto m_serverLifetime = winrt::create_instance<Server::LifetimeManager>(
			guid{ L"898F12B7-4BB0-4279-B3B1-126440D7CB7A" }, CLSCTX_LOCAL_SERVER
		);
		return m_serverLifetime;
	}
}
