#include "pch.h"
#include "CornerActions.h"
#include "TrayCornerTracker.h"
#include <array>

namespace winrt::HotCorner::Server::Tracking {
	static constexpr INPUT VirtualKeyInput(const WORD virtualKeyCode, const DWORD flags = 0) {
		return INPUT{
			.type = INPUT_KEYBOARD,
			.ki{
				.wVk = virtualKeyCode,
				.dwFlags = flags,
			}
		};
	}

	template<size_t InputSize>
	constexpr bool Inject(std::array<INPUT, InputSize>& input) {
		const auto inSize = static_cast<UINT>(InputSize);
		return SendInput(inSize, input.data(), sizeof(INPUT)) == inSize;
	}

#define KEYBOARD_INPUT(NAME, VKCODE) constexpr INPUT NAME##Down{ VirtualKeyInput(VKCODE) }; constexpr INPUT NAME##Up{ VirtualKeyInput(VKCODE, KEYEVENTF_KEYUP) }

	KEYBOARD_INPUT(LWin, VK_LWIN);
	KEYBOARD_INPUT(Tab, VK_TAB);
	KEYBOARD_INPUT(DKey, 0x44);
	KEYBOARD_INPUT(SKey, 0x53);

	std::array<INPUT, 4> SearchInput = { LWinDown, SKeyDown, SKeyUp, LWinUp, };
	std::array<INPUT, 4> ShowDesktopInput = { LWinDown, DKeyDown, DKeyUp, LWinUp, };
	std::array<INPUT, 4> TaskViewInput = { LWinDown, TabDown, TabUp, LWinUp, };

	bool RunAction(const Settings::CornerAction action) noexcept {
		using ActionT = Settings::CornerAction;
		using TCT = Tracking::TrayCornerTracker;

		switch (action) {
		case ActionT::TaskView:
			return Inject(TaskViewInput);

		case ActionT::Start:
			return TCT::Current().Post(WM_SYSCOMMAND, SC_TASKLIST);

		case ActionT::Search:
			return Inject(SearchInput);

		case ActionT::GoToDesktop:
			return Inject(ShowDesktopInput);
		}
		return false;
	}
}
