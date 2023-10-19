#include "pch.h"
#include "main.h"

#include "LifetimeManager.h"
#include "Tracking/TrayCornerTracker.h"

import server;

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

	extern "C" int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
		SetModule(instance);
		winrt::init_apartment();

		const auto cookies = server::register_classes<implementation::LifetimeManager>();
		CoResumeClassObjects();

		const auto result = Tracking::TrayCornerTracker::Current().RunMessageLoop();
		server::unregister_classes(cookies);

		return static_cast<int>(result);
	}
}
