#pragma once
#include <spdlog/spdlog.h>

namespace winrt::HotCorner::Server {
	/**
	 * @brief A light wrapper for a Win32 window class.
	*/
	class WindowClass {
		WNDCLASSEX cls;

		ATOM cookie;

	public:
		WindowClass(HINSTANCE instance, std::wstring_view name, WNDPROC procedure, int extra = 0) noexcept :
			cls({
				.cbSize = sizeof(WNDCLASSEX),
				.lpfnWndProc = procedure,
				.cbWndExtra = extra,
				.hInstance = instance,
				.lpszClassName = name.data()
				}
			),
			cookie(0)
		{
			if (!TryRegister()) {
				spdlog::critical("Failed to register window class. Error: {}", GetLastError());
			}
		}

		inline ~WindowClass() noexcept {
			if (IsRegistered()) {
				if (!TryUnregister()) {
					spdlog::error("Failed to unregister window class. Error: {}", GetLastError());
				}
			}
		}

		constexpr const WNDCLASSEXW& Get() const noexcept {
			return cls;
		}

		constexpr bool IsRegistered() const noexcept {
			return cookie != 0;
		}

		inline LPCTSTR ClassAtom() const noexcept {
			return reinterpret_cast<LPCTSTR>(static_cast<INT_PTR>(cookie));
		}

		inline bool TryRegister() noexcept {
			cookie = RegisterClassEx(&cls);
			return cookie != 0;
		}

		inline bool TryUnregister() noexcept {
			const auto atom = ClassAtom();
			cookie = 0;
			return UnregisterClass(atom, cls.hInstance);
		}

		constexpr LPCWSTR ClassName() const noexcept {
			return cls.lpszClassName;
		}

		constexpr WNDPROC WindowProcedure() const noexcept {
			return cls.lpfnWndProc;
		}

		constexpr HINSTANCE Instance() const noexcept {
			return cls.hInstance;
		}

		constexpr uint32_t Styles() const noexcept {
			return cls.style;
		}

		constexpr HICON Icon() const noexcept {
			return cls.hIcon;
		}

		constexpr HICON SmallIcon() const noexcept {
			return cls.hIconSm;
		}

		constexpr HCURSOR Cursor() const noexcept {
			return cls.hCursor;
		}

		constexpr HBRUSH BackgroundBrush() const noexcept {
			return cls.hbrBackground;
		}

		constexpr LPCWSTR MenuName() const noexcept {
			return cls.lpszMenuName;
		}
	};
}
