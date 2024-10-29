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
	using InputSequence = std::array<INPUT, InputSize>;
	
	template<size_t InputSize>
	inline bool Inject(InputSequence<InputSize>& input) {
		const auto inSize = static_cast<UINT>(InputSize);
		return SendInput(inSize, input.data(), sizeof(INPUT)) == inSize;
	}

#define KEYBOARD_INPUT(NAME, VKCODE) constexpr INPUT NAME##Down{ VirtualKeyInput(VKCODE) }; constexpr INPUT NAME##Up{ VirtualKeyInput(VKCODE, KEYEVENTF_KEYUP) }

	KEYBOARD_INPUT(LWin, VK_LWIN);
	KEYBOARD_INPUT(Tab, VK_TAB);
	KEYBOARD_INPUT(Ctrl, VK_CONTROL);
	KEYBOARD_INPUT(Right, VK_RIGHT);
	KEYBOARD_INPUT(Left, VK_LEFT);
	KEYBOARD_INPUT(AKey, 0x41);
	KEYBOARD_INPUT(DKey, 0x44);
	KEYBOARD_INPUT(SKey, 0x53);

	InputSequence<4> QuickSettingsInput = { LWinDown, AKeyDown, AKeyUp, LWinUp, };
	InputSequence<4> SearchInput = { LWinDown, SKeyDown, SKeyUp, LWinUp, };
	InputSequence<4> ShowDesktopInput = { LWinDown, DKeyDown, DKeyUp, LWinUp, };
	InputSequence<4> TaskViewInput = { LWinDown, TabDown, TabUp, LWinUp, };
	InputSequence<6> PreviousVirtualDesktopInput = { LWinDown, CtrlDown, LeftDown, LeftUp, CtrlUp, LWinUp, };
	InputSequence<6> NextVirtualDesktopInput = { LWinDown, CtrlDown, RightDown, RightUp, CtrlUp, LWinUp, };

	bool RunAction(const TrayIcon& icon, Settings::CornerAction action) noexcept {
		using enum Settings::CornerAction;
		using TCT = Tracking::TrayCornerTracker;

		switch (action) {
		case TaskView:
			return Inject(TaskViewInput);

		case Start:
			return icon.Post(WM_SYSCOMMAND, SC_TASKLIST);

		case Search:
			return Inject(SearchInput);

		case GoToDesktop:
			return Inject(ShowDesktopInput);

		case QuickSettings:
			return Inject(QuickSettingsInput);

		case PreviousVirtualDesktop:
			return Inject(PreviousVirtualDesktopInput);

		case NextVirtualDesktop:
			return Inject(NextVirtualDesktopInput);
		}
		return false;
	}
}
