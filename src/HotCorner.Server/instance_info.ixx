module;
#include "pch.h"

export module instance_info;

namespace instance_info {
	namespace {
		static HINSTANCE m_curr{};
	}

	export inline void set(HINSTANCE instance) noexcept {
		m_curr = instance;
	}

	export inline HINSTANCE current_instance() noexcept {
		return m_curr;
	}
}
