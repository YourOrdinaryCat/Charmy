#include "pch.h"
#include "UxTheme.h"

#include <Version.h>
#include <wil/resource.h>

namespace winrt::HotCorner::Server::Undocumented {
	ShellTheme GetCurrentShellTheme() noexcept {
		// Before 1903, the default shell theme was dark, no light mode
		if (!IsAtLeast1903()) {
			return ShellTheme::Dark;
		}

		// https://github.com/TranslucentTB/TranslucentTB/blob/0726b0afbdf83eb579537d59bb36076a9071ec06/TranslucentTB/dynamicloader.hpp#L26C4-L26C4
		static wil::unique_hmodule m_UxTheme{};
		m_UxTheme.reset(LoadLibraryEx(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));

		//TODO: Handle failure
		if (m_UxTheme) {
			static PFN_SHOULD_SYSTEM_USE_DARK_MODE m_Ssudm =
				reinterpret_cast<PFN_SHOULD_SYSTEM_USE_DARK_MODE>(GetProcAddress(m_UxTheme.get(), MAKEINTRESOURCEA(138)));

			if (m_Ssudm) {
				return m_Ssudm() ? ShellTheme::Dark : ShellTheme::Light;
			}

			OutputDebugString(L"Failed to get address of ShouldSystemUseDarkMode\n");
		}
		else {
			OutputDebugString(L"Unable to load uxtheme.dll\n");
		}

		return ShellTheme::Unknown;
	}
}
