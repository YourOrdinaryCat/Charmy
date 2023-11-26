#include "pch.h"
#include "CornerActions.h"
#include "TrayCornerTracker.h"

namespace winrt::HotCorner::Server::Tracking {
	namespace {
		constexpr INPUT VirtualKeyInput(const WORD virtualKeyCode, const DWORD flags = 0) {
			return INPUT{
				.type = INPUT_KEYBOARD,
				.ki{
					.wVk = virtualKeyCode,
					.dwFlags = flags,
				}
			};
		}

#define KEYBOARD_INPUT(NAME, VKCODE) constexpr INPUT NAME##Down{ VirtualKeyInput(VKCODE) }; constexpr INPUT NAME##Up{ VirtualKeyInput(VKCODE, KEYEVENTF_KEYUP) }

		KEYBOARD_INPUT(LWin, VK_LWIN);
		KEYBOARD_INPUT(Tab, VK_TAB);
		KEYBOARD_INPUT(DKey, 0x44);
		KEYBOARD_INPUT(SKey, 0x53);

		std::array<INPUT, 4> SearchInput = { LWinDown, SKeyDown, SKeyUp, LWinUp, };
		std::array<INPUT, 4> ShowDesktopInput = { LWinDown, DKeyDown, DKeyUp, LWinUp, };
		std::array<INPUT, 4> TaskViewInput = { LWinDown, TabDown, TabUp, LWinUp, };
	}

	std::optional<std::function<bool()>> GetDelegate(Settings::CornerAction action) noexcept {
		switch (action) {
		case Settings::CornerAction::TaskView:
			return OpenTaskView;

		case Settings::CornerAction::Start:
			return OpenStart;

		case Settings::CornerAction::Search:
			return OpenSearch;

		case Settings::CornerAction::GoToDesktop:
			return ToggleDesktop;
		}
		return std::nullopt;
	}

	bool OpenTaskView() noexcept {
		const auto inputs = static_cast<UINT>(TaskViewInput.size());
		return SendInput(inputs, TaskViewInput.data(), sizeof(INPUT)) == inputs;
	}

	bool OpenStart() noexcept {
		return Tracking::TrayCornerTracker::Current().Post(WM_SYSCOMMAND, SC_TASKLIST);
	}

	bool OpenSearch() noexcept {
		const auto inputs = static_cast<UINT>(SearchInput.size());
		return SendInput(inputs, SearchInput.data(), sizeof(INPUT)) == inputs;
	}

	bool ToggleDesktop() noexcept {
		const auto inputs = static_cast<UINT>(ShowDesktopInput.size());
		return SendInput(inputs, ShowDesktopInput.data(), sizeof(INPUT)) == inputs;
	}
}
