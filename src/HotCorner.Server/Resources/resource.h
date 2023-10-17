#pragma once

#define IDS_TRAY_TOOLTIP 60001

namespace winrt::HotCorner::Server::Resources {
	template<int Length>
	std::array<WCHAR, Length> GetString(HINSTANCE instance, UINT id) noexcept {
		static_assert(Length > 0, "Buffer size must be greater ");
		std::array<WCHAR, Length> buffer{};

		//TODO: Handle possible failure here
		LoadStringW(instance, id, buffer.data(), Length);

		return buffer;
	}
}
