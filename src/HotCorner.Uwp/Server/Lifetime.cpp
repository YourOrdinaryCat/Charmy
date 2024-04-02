#include "pch.h"
#include "Lifetime.h"

#include <mutex>

namespace winrt::HotCorner::Uwp::Lifetime {
	static constexpr guid CLSID_LifetimeManager{ L"898F12B7-4BB0-4279-B3B1-126440D7CB7A" };
	static Server::LifetimeManager m_lifetime{ nullptr };
	static std::once_flag m_lockServerFlag;

	const Server::LifetimeManager& Current() {
		std::call_once(m_lockServerFlag, []() {
			m_lifetime = winrt::create_instance<Server::LifetimeManager>(
				CLSID_LifetimeManager, CLSCTX_LOCAL_SERVER
			);
			m_lifetime.LockServer(GetCurrentProcessId());
		});

		return m_lifetime;
	}
}
