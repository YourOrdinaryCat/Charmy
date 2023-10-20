#pragma once
#include <array>
#include <CommCtrl.h>
#include <string_view>
#include "main.h"
#include "Resources/ids.h"

namespace winrt::HotCorner::Server::Resources {
	template<int Length>
	inline void GetString(UINT id, wchar_t* buffer) noexcept {
		static_assert(Length > 0, "Buffer size must be greater than 0");

		//TODO: Handle possible failure here
		LoadStringW(Current::Module(), id, buffer, Length);
	}

	template<int Length>
	inline constexpr std::array<WCHAR, Length> GetString(UINT id) noexcept {
		std::array<WCHAR, Length> buffer{};
		GetString<Length>(id, buffer.data());
		return buffer;
	}

	inline void GetSmallIcon(UINT id, HICON* icon) noexcept {
		//TODO: Handle possible failure here
		LoadIconMetric(Current::Module(), MAKEINTRESOURCE(id), LIM_SMALL, icon);
	}

	inline void GetSmallIcon(const wchar_t* id, HICON* icon) noexcept {
		//TODO: Handle possible failure here
		LoadIconMetric(Current::Module(), id, LIM_SMALL, icon);
	}
}
