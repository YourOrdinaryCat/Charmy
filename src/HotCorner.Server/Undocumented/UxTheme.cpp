#include "pch.h"
#include "UxTheme.h"
#include <Logging.h>
#include <Version.h>
#include <wil/resource.h>

namespace winrt::HotCorner::Server::Undocumented {
	static wil::unique_hmodule m_UxTheme{};
	static PFN_SHOULD_SYSTEM_USE_DARK_MODE m_Ssudm = nullptr;

	ShellTheme GetCurrentShellTheme() noexcept {
		// Before 1903, the default shell theme was dark, no light mode
		if (!IsAtLeast1903()) {
			return ShellTheme::Dark;
		}

		// https://github.com/TranslucentTB/TranslucentTB/blob/0726b0afbdf83eb579537d59bb36076a9071ec06/TranslucentTB/dynamicloader.hpp#L26C4-L26C4
		if (!m_UxTheme) {
			m_UxTheme.reset(LoadLibraryEx(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
		}

		if (m_UxTheme) {
			if (!m_Ssudm) {
				m_Ssudm = reinterpret_cast<PFN_SHOULD_SYSTEM_USE_DARK_MODE>(
					GetProcAddress(m_UxTheme.get(), MAKEINTRESOURCEA(138)));
			}

			if (m_Ssudm) {
				return m_Ssudm() ? ShellTheme::Dark : ShellTheme::Light;
			}

			SPDLOG_LAST_ERROR(spdlog::level::warn, "Failed to get address of ShouldSystemUseDarkMode");
		}
		else {
			SPDLOG_LAST_ERROR(spdlog::level::warn, "Unable to load uxtheme.dll");
		}

		return ShellTheme::Unknown;
	}
}
