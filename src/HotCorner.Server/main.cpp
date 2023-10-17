#include "pch.h"
#include "main.h"

#include "LifetimeManager.h"

import server;

namespace srv = winrt::HotCorner::Server;
namespace impl = winrt::HotCorner::Server::implementation;
namespace inst = winrt::HotCorner::Server::Current;

namespace winrt::HotCorner::Server::Current {
	namespace {
		HINSTANCE m_instance{};
	}

	void SetModule(HINSTANCE instance) {
		m_instance = instance;
	}

	HINSTANCE Module() noexcept {
		return m_instance;
	}

	srv::TrayIcon& Notification() noexcept {
		static auto m_trayIcon = srv::TrayIcon(m_instance, __uuidof(IUnknown));
		return m_trayIcon;
	}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
	inst::SetModule(instance);
	winrt::init_apartment();

	const auto cookies = server::register_classes<impl::LifetimeManager>();
	CoResumeClassObjects();

	const auto result = inst::Notification().RunMessageLoop();
	server::unregister_classes(cookies);

	return static_cast<int>(result);
}
