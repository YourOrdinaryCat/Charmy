#pragma once
#include <minwindef.h>

/**
 * @brief Windows doesn't play fair with tray icons. The built-in ones are
 *        able to respond to theme changes and the like, but they unfortunately
 *        do so through this undocumented method. All this thing does is return
 *        true if the shell is using dark mode, so why they keep it undocumented is
 *        beyond me. Many thanks to the TranslucentTB authors for their efforts - if
 *        it wasn't for their code, I would be reading the registry for this.
 *        https://github.com/TranslucentTB/TranslucentTB/blob/0726b0afbdf83eb579537d59bb36076a9071ec06/TranslucentTB/dynamicloader.hpp
 *
 * @returns TRUE if the shell is using dark mode, FALSE otherwise.
 *
 * @remarks THIS IS UNDOCUMENTED! Extra care and very careful error handling should be
 *          applied when using this.
*/
typedef BOOL(WINAPI* PFN_SHOULD_SYSTEM_USE_DARK_MODE)();

/**
 * @brief THIS IS UNDOCUMENTED TERRITORY! Extra care and very careful error handling
 *        should be applied when using anything from this namespace. As soon as a
 *        documented solution to the problem exists, this should go away.
*/
namespace winrt::HotCorner::Server::Undocumented {
	enum class ShellTheme {
		Light = 0,
		Dark = 1,
		Unknown = 2
	};

	/**
	 * @brief Gets the currently applied shell theme.
	 * 
	 * @returns If unable to determine the theme, returns Unknown. Otherwise, returns
	 *          the theme that is currently applied.
	*/
	ShellTheme GetCurrentShellTheme() noexcept;
}
