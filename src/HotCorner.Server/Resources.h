#pragma once
#include "Resources/ids.h"
#include <array>
#include <CommCtrl.h>
#include <string_view>

namespace winrt::HotCorner::Server::Resources {
	template<int Length>
	inline int GetString(HINSTANCE instance, UINT id, wchar_t* buffer) noexcept {
		static_assert(Length > 0, "Buffer size must be greater than 0");

		return LoadStringW(instance, id, buffer, Length);
	}

	inline HRESULT GetSmallIcon(HINSTANCE instance, UINT id, HICON* icon) noexcept {
		return LoadIconMetric(instance, MAKEINTRESOURCE(id), LIM_SMALL, icon);
	}

	inline HRESULT GetSmallIcon(HINSTANCE instance, std::wstring_view id, HICON* icon) noexcept {
		return LoadIconMetric(instance, id.data(), LIM_SMALL, icon);
	}
}
