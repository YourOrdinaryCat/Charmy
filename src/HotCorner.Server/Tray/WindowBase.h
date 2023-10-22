#pragma once
#include "WindowClass.h"

namespace winrt::HotCorner::Server {
	/**
	 * @brief A light wrapper for a Win32 window with a message handler.
	*/
	template<typename Derived>
	class WindowBase {
		static LRESULT CALLBACK BaseProtocol(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam) noexcept
		{
			Derived* pThis = NULL;

			if (message != WM_NCCREATE) {
				pThis = reinterpret_cast<Derived*>(GetWindowLongPtr(hwnd, 0));
			}
			else {
				const auto pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
				pThis = static_cast<Derived*>(pCreate->lpCreateParams);

				SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(pThis));
				pThis->m_window = hwnd;
			}

			if (pThis) {
				return pThis->HandleMessage(message, wParam, lParam);
			}

			return DefWindowProc(hwnd, message, wParam, lParam);
		}

	protected:
		const WindowClass m_windowClass;

		bool m_closed = false;
		HWND m_window;

		/**
		 * @brief Defines a window message guaranteed to be unique throughout the system.
		 *        Tends to be used for communication between different applications.
		*/
		std::optional<UINT> RegisterMessage(std::wstring_view message) const noexcept {
			const UINT msg = RegisterWindowMessage(message.data());
			if (msg) {
				return msg;
			}
			else {
				//TODO: Handle failure
				OutputDebugString(L"Failed to register window message\n");
				return std::nullopt;
			}
		}

		/**
		 * @brief The window's message handler. Override this in a derived
		 *        class to handle messages on your own - by default, all
		 *        calls to this method are forwarded to DefWindowProcW.
		*/
		inline virtual LRESULT HandleMessage(
			UINT message,
			WPARAM wParam,
			LPARAM lParam) noexcept
		{
			if (message == WM_DESTROY) [[unlikely]] {
				PostQuitMessage(0);
			}

			return DefWindowProc(m_window, message, wParam, lParam);
		}

		WindowBase(HINSTANCE instance, std::wstring_view windowClass, std::wstring_view windowName) noexcept :
			m_windowClass(instance, windowClass, &WindowBase::BaseProtocol, sizeof(this)),
			m_window(CreateWindow(
				m_windowClass.ClassAtom(),
				windowName.data(),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				NULL,
				NULL,
				m_windowClass.Instance(),
				this
			))
		{
			if (!m_window) {
				//TODO: Handle failure
				OutputDebugString(L"Failed to create window\n");
			}
		}

		virtual ~WindowBase() noexcept {
			if (!m_closed) {
				Close();
			}
		}

	public:
		/**
		 * @brief Posts a message in this window's message queue. This method returns
		 *        without waiting for the message to be processed.
		*/
		inline bool Post(UINT message) const noexcept {
			return PostMessage(m_window, message, 0, 0);
		}

		/**
		 * @brief Sends a message in this window's message queue.
		 *
		 * @returns The result after the message has been processed.
		*/
		inline LRESULT Send(UINT message) const noexcept {
			return SendMessage(m_window, message, 0, 0);
		}

		/**
		 * @brief Runs the window's message loop until the window is destroyed.
		*/
		WPARAM RunMessageLoop() const noexcept {
			MSG msg;
			BOOL state;

			while ((state = GetMessage(&msg, nullptr, 0, 0)) != 0) {
				if (state != -1) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else {
					//TODO: Handle failure
					OutputDebugString(L"Unspecified failure in window message loop\n");
					break;
				}
			}

			return msg.wParam;
		}

		/**
		 * @brief Closes the window by sending WM_CLOSE. Usually, this results
		 *        in destruction of the window.
		*/
		void Close() noexcept {
			const bool result = Post(WM_CLOSE);

			if (result) {
				m_closed = true;
			}
			else {
				//TODO: Handle failure
				OutputDebugString(L"Failed to close window\n");
			}
		}
	};
}
