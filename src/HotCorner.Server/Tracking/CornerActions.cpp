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
		return Inject(TaskViewInput);
	}

	bool OpenStart() noexcept {
		return Tracking::TrayCornerTracker::Current().Post(WM_SYSCOMMAND, SC_TASKLIST);
	}

	bool OpenSearch() noexcept {
		return Inject(SearchInput);
	}

	bool ToggleDesktop() noexcept {
		return Inject(ShowDesktopInput);
	}
}
