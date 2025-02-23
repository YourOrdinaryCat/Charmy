#pragma once
#include "WindowClass.h"
#include <async/task_completion_source.h>
#include <Coro.h>
#include <Logging.h>

namespace winrt::HotCorner::Server {
	/**
	 * @brief A light wrapper for a Win32 window with a message handler.
	*/
	class WindowBase {
		static LRESULT CALLBACK BaseProtocol(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam) noexcept
		{
			WindowBase* pThis = nullptr;

			if (message != WM_NCCREATE) {
				pThis = reinterpret_cast<WindowBase*>(GetWindowLongPtr(hwnd, 0));
			}
			else {
				const auto pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
				pThis = static_cast<WindowBase*>(pCreate->lpCreateParams);

				SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(pThis));
				pThis->m_window = hwnd;
			}

			if (pThis) {
				return pThis->HandleMessage(message, wParam, lParam);
			}

			return DefWindowProc(hwnd, message, wParam, lParam);
		}

		using AsyncMessageCompletionEvent = async::task_completion_source<std::optional<LRESULT>>;

	protected:
		const WindowClass m_windowClass;

		bool m_closed = false;
		HWND m_window;

		/**
		 * @brief Defines a window message guaranteed to be unique throughout the system.
		 *        Tends to be used for communication between different applications.
		*/
		std::optional<UINT> RegisterMessage(std::wstring_view message) const noexcept {
			SetLastError(0);
			const UINT msg = RegisterWindowMessage(message.data());

			if (msg) {
				return msg;
			}
			else {
				SPDLOG_LAST_ERROR(spdlog::level::err, "Failed to register window message");
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

		/**
		 * @brief A simple helper method that handles messages sent by SendAsync.
		 *
		 * @returns The original result that was passed in.
		*/
		inline LRESULT HandleAsyncMessage(LPARAM lParam, LRESULT result) noexcept {
			if (lParam) {
				auto evt = reinterpret_cast<AsyncMessageCompletionEvent*>(lParam);
				evt->set_value(result);
			}
			return result;
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
				SPDLOG_LAST_ERROR(spdlog::level::critical, "Failed to create window");
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
		inline bool Post(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const noexcept {
			return PostMessage(m_window, message, wParam, lParam);
		}

		/**
		 * @brief Sends a message in this window's message queue.
		 *
		 * @returns The result after the message has been processed.
		*/
		inline LRESULT Send(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const noexcept {
			return SendMessage(m_window, message, wParam, lParam);
		}

		/**
		 * @brief Sends a message in this window's message queue to be processed
		 *        asynchronously. This does not use the SendMessageCallback function -
		 *        SendNotifyMessage is called with the message, and a completion event
		 *        (pointed to by lParam). Message processing code is expected to call
		 *        HandleAsyncMessage with the wParam as the first argument, and processing
		 *        result as the second.
		 *
		 * @returns A task that, on completion, returns the result of message processing.
		*/
		inline async::tcs_task<std::optional<LRESULT>> SendAsync(UINT message, WPARAM wParam = 0) const {
			auto tce = std::make_shared<AsyncMessageCompletionEvent>();

			const BOOL result = SendNotifyMessage(
				m_window, message,
				wParam,
				reinterpret_cast<LPARAM>(tce.get())
			);

			if (!result) {
				SPDLOG_LAST_ERROR(spdlog::level::err, "Unable to send message");
				tce->set_value(std::nullopt);
			}

			return async::tcs_task{ std::move(tce) };
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
					SPDLOG_LAST_ERROR(spdlog::level::critical, "Unspecified failure in window message loop");
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
			SetLastError(0);
			const bool result = Post(WM_CLOSE);

			if (result) {
				m_closed = true;
			}
			else {
				SPDLOG_LAST_ERROR(spdlog::level::err, "Failed to close window");
			}
		}
	};
}
