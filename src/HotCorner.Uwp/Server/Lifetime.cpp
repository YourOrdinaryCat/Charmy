#include "pch.h"
#include "Lifetime.h"

namespace winrt::HotCorner::Uwp::Lifetime {
	static constexpr guid CLSID_LifetimeManager{ L"898F12B7-4BB0-4279-B3B1-126440D7CB7A" };
	static Server::LifetimeManager m_lifetime{ nullptr };

	bool Started() noexcept {
		return m_lifetime != nullptr;
	}

	const Server::LifetimeManager& Current() {
		if (!Started()) {
			m_lifetime = winrt::create_instance<Server::LifetimeManager>(
				CLSID_LifetimeManager, CLSCTX_LOCAL_SERVER
			);
			m_lifetime.LockServer(GetCurrentProcessId());
		}
		return m_lifetime;
	}
}
