#pragma once
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <CommCtrl.h>

#define IDI_TRAYICON 101

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

	HICON GetSmallIcon(HINSTANCE instance, UINT id) noexcept {
		HICON icon{};
		LoadIconMetric(instance, MAKEINTRESOURCE(id), LIM_SMALL, &icon);

		return icon;
	}
}
