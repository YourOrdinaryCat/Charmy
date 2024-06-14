#pragma once
#include <array>
#include <CommCtrl.h>
#include <string_view>
#include "Resources/ids.h"

namespace winrt::HotCorner::Server::Resources {
	template<int Length>
	inline void GetString(HINSTANCE instance, UINT id, wchar_t* buffer) noexcept {
		static_assert(Length > 0, "Buffer size must be greater than 0");

		//TODO: Handle possible failure here
		LoadStringW(instance, id, buffer, Length);
	}

	template<int Length>
	inline constexpr std::array<WCHAR, Length> GetString(UINT id) noexcept {
		std::array<WCHAR, Length> buffer{};
		GetString<Length>(id, buffer.data());
		return buffer;
	}

	inline void GetSmallIcon(HINSTANCE instance, UINT id, HICON* icon) noexcept {
		//TODO: Handle possible failure here
		LoadIconMetric(instance, MAKEINTRESOURCE(id), LIM_SMALL, icon);
	}

	inline void GetSmallIcon(HINSTANCE instance, const wchar_t* id, HICON* icon) noexcept {
		//TODO: Handle possible failure here
		LoadIconMetric(instance, id, LIM_SMALL, icon);
	}
}
