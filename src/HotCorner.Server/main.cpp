#include "pch.h"
#include "main.h"

#include "LifetimeManager.h"

import server;

namespace srv = winrt::HotCorner::Server;
namespace impl = winrt::HotCorner::Server::implementation;
namespace inst = winrt::HotCorner::Server::Current;

namespace winrt::HotCorner::Server::Current {
	HINSTANCE Module() noexcept {
		static auto m_curr = GetModuleHandle(NULL);
		return m_curr;
	}

	srv::TrayIcon& Notification() noexcept {
		static auto m_trayIcon = srv::TrayIcon(Module(), __uuidof(IUnknown));
		return m_trayIcon;
	}
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
	winrt::init_apartment();

	const auto cookies = server::register_classes<impl::LifetimeManager>();
	CoResumeClassObjects();

	const auto result = inst::Notification().RunMessageLoop();
	server::unregister_classes(cookies);

	return static_cast<int>(result);
}
