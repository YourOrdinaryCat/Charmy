#include "pch.h"
#include "CornerActions.h"
#include <ShlDisp.h>
#include "TrayCornerTracker.h"

namespace winrt::HotCorner::Server::Tracking {
	namespace {
		IShellDispatch5* ShellDispatch() noexcept {
			static IShellDispatch5* m_shellDispatch = nullptr;
			if (m_shellDispatch == nullptr) {
				const auto hr = CoCreateInstance(
					CLSID_Shell,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_IShellDispatch5,
					reinterpret_cast<void**>(&m_shellDispatch)
				);

				if (hr != S_OK) {
					//TODO: Something is very wrong
					OutputDebugString(L"Unable to create instance of IShellDispatch5\n");
				}
			}
			return m_shellDispatch;
		}

		constexpr INPUT VirtualKeyInput(const WORD virtualKeyCode, const DWORD flags = 0) {
			return INPUT{
				.type = INPUT_KEYBOARD,
				.ki{
					.wVk = virtualKeyCode,
					.dwFlags = flags,
				}
			};
		}

		constexpr INPUT LWinDown{ VirtualKeyInput(VK_LWIN) };
		constexpr INPUT LWinUp{ VirtualKeyInput(VK_LWIN, KEYEVENTF_KEYUP) };

		constexpr INPUT SKeyDown{ VirtualKeyInput(0x53) };
		constexpr INPUT SKeyUp{ VirtualKeyInput(0x53, KEYEVENTF_KEYUP) };

		std::array<INPUT, 4> SearchInput = { LWinDown, SKeyDown, SKeyUp, LWinUp, };
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
		const auto hr = ShellDispatch()->WindowSwitcher();
		return SUCCEEDED(hr);
	}

	bool OpenStart() noexcept {
		return Tracking::TrayCornerTracker::Current().Post(WM_SYSCOMMAND, SC_TASKLIST);
	}

	bool OpenSearch() noexcept {
		const auto inputs = static_cast<UINT>(SearchInput.size());
		return SendInput(inputs, SearchInput.data(), sizeof(INPUT)) == inputs;
	}

	bool ToggleDesktop() noexcept {
		return ShellDispatch()->MinimizeAll() >= 0;
	}
}
