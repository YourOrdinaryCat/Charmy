#include "pch.h"
#include "CornerTracker.h"

#include "../main.h"

namespace winrt::HotCorner::Server::CornerTracker {
	namespace {
		HANDLE m_cornerThread = INVALID_HANDLE_VALUE;
		HHOOK m_mouseHook = NULL;

		std::vector<RECT> m_hotCorners{};
		std::array<INPUT, 4> m_cornerInput = {
			INPUT { INPUT_KEYBOARD, { VK_LWIN, 0 } },
			{ INPUT_KEYBOARD, { VK_TAB, 0 } },
			{ INPUT_KEYBOARD, { VK_TAB, KEYEVENTF_KEYUP } },
			{ INPUT_KEYBOARD, { VK_LWIN, KEYEVENTF_KEYUP } },
		};

		const RECT m_cornerOffsets = {
			-10,
			-10,
			+10,
			+10,
		};

		bool IsPointWithinHotCorner(const POINT& pt) {
			for (const auto& rect : m_hotCorners) {
				if (PtInRect(&rect, pt)) {
					return true;
				}
			}
			return false;
		}

		BOOL CALLBACK MonitorEnumProc(HMONITOR, HDC, LPRECT lprcMonitor, LPARAM) {
			RECT offset{
				.left = lprcMonitor->left + m_cornerOffsets.left,
				.top = lprcMonitor->top + m_cornerOffsets.top,
				.right = lprcMonitor->left + m_cornerOffsets.right,
				.bottom = lprcMonitor->top + m_cornerOffsets.bottom,
			};
			m_hotCorners.emplace_back(offset);

			return TRUE;
		}

		inline bool IsKeyDown(BYTE key) {
			return key & 0x80;
		}

		// This thread runs when the cursor enters the hot corner, and waits to see if the cursor stays in the corner.
		// If the mouse leaves while we're waiting, the thread is just terminated.
		ULONG WINAPI CornerHotFunc(LPVOID) {
			// Check if a mouse putton is pressed, maybe a drag operation?
			if (GetKeyState(VK_LBUTTON) < 0 || GetKeyState(VK_RBUTTON) < 0) {
				return 0;
			}

			std::array<BYTE, 256> keyState{};

			// Check if any modifier keys are pressed.
			if (GetKeyboardState(keyState.data())) {
				if (IsKeyDown(keyState[VK_SHIFT]) || IsKeyDown(keyState[VK_CONTROL]) ||
					IsKeyDown(keyState[VK_MENU]) || IsKeyDown(keyState[VK_LWIN]) ||
					IsKeyDown(keyState[VK_RWIN]))
				{
					return 0;
				}
			}

			// Verify the corner is still hot
			POINT pt;
			if (GetCursorPos(&pt) == FALSE) {
				return 1;
			}

			// Check coords
			if (IsPointWithinHotCorner(pt)) {
				if (SendInput(4, m_cornerInput.data(), sizeof(INPUT)) != 4) {
					return 1;
				}
			}

			return 0;
		}

		LRESULT CALLBACK MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
			// If the mouse hasn't moved, we're done
			if (wParam != WM_MOUSEMOVE) {
				goto next_hook;
			}

			// If no hot corner is active, update monitors positions (preventing data race)
			if (m_cornerThread == INVALID_HANDLE_VALUE) {
				m_hotCorners.clear();
				EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, NULL);
			}

			// Need position compatible with EnumDisplayMonitors
			POINT pt;
			if (GetCursorPos(&pt) == FALSE) {
				goto next_hook;
			}

			// Check if the cursor is hot or cold
			if (!IsPointWithinHotCorner(pt)) {
				// The corner is cold, and was cold before
				if (m_cornerThread == INVALID_HANDLE_VALUE) {
					goto next_hook;
				}

				// The corner is cold, but was previously hot
				TerminateThread(m_cornerThread, 0);
				CloseHandle(m_cornerThread);

				// Reset state.
				m_cornerThread = INVALID_HANDLE_VALUE;

				goto next_hook;
			}

			// The corner is hot, check if it was already hot
			if (m_cornerThread != INVALID_HANDLE_VALUE) {
				goto next_hook;
			}

			// Check if a mouse putton is pressed, maybe a drag operation?
			if (GetKeyState(VK_LBUTTON) < 0 || GetKeyState(VK_RBUTTON) < 0) {
				goto next_hook;
			}

			// The corner is hot, and was previously cold. Here we start a thread to
			// monitor if the mouse lingers
			m_cornerThread = CreateThread(NULL, 0, CornerHotFunc, NULL, 0, NULL);

		next_hook:
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}
	}

	StartupResult Start() noexcept {
		if (m_mouseHook != NULL) {
			return StartupResult::AlreadyStarted;
		}

		m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, NULL, 0);
		if (m_mouseHook != NULL) {
			SetPriorityClass(Current::Module(), REALTIME_PRIORITY_CLASS);
			return StartupResult::Started;
		}

		return StartupResult::Failed;
	}

	StopResult Stop() noexcept {
		if (m_mouseHook == NULL) {
			return StopResult::NotRunning;
		}

		if (UnhookWindowsHookEx(m_mouseHook)) {
			m_mouseHook = NULL;
			SetPriorityClass(Current::Module(), NORMAL_PRIORITY_CLASS);

			return StopResult::Stopped;
		}

		return StopResult::Failed;
	}
}
