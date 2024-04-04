#include "pch.h"
#include "Lifetime.h"

namespace winrt::HotCorner::Uwp::Lifetime {
	static constexpr guid CLSID_LifetimeManager{ L"898F12B7-4BB0-4279-B3B1-126440D7CB7A" };

	Server::LifetimeManager Current() {
		static Server::LifetimeManager m_lifetime =
			winrt::create_instance<Server::LifetimeManager>(
				CLSID_LifetimeManager, CLSCTX_LOCAL_SERVER
			);

		return m_lifetime;
	}
}
