#include "pch.h"
#include "Lifetime.h"

namespace winrt::HotCorner::Uwp::Lifetime {
	Server::LifetimeManager Current() {
		constexpr guid iid = winrt::guid_of<Server::LifetimeManager>();
		static Server::LifetimeManager m_lifetime =
			winrt::create_instance<Server::LifetimeManager>(iid, CLSCTX_LOCAL_SERVER);

		return m_lifetime;
	}
}
