#pragma once

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
				//TODO: Handle failure
				OutputDebugString(L"Failed to register window class\n");
			}
		}

		inline ~WindowClass() noexcept {
			if (IsRegistered()) {
				if (!TryUnregister()) {
					//TODO: Handle failure
					OutputDebugString(L"Failed to unregister window class\n");
				}
			}
		}

		inline const WNDCLASSEXW& Get() const noexcept {
			return cls;
		}

		inline bool IsRegistered() const noexcept {
			return cookie != 0;
		}

		inline LPCWSTR ClassAtom() const noexcept {
			return MAKEINTATOM(cookie);
		}

		inline bool TryRegister() noexcept {
			cookie = RegisterClassEx(&cls);
			return cookie != 0;
		}

		inline bool TryUnregister() noexcept {
			const auto atom = MAKEINTATOM(cookie);
			cookie = 0;
			return UnregisterClassW(atom, cls.hInstance);
		}

		inline LPCWSTR ClassName() const noexcept {
			return cls.lpszClassName;
		}

		inline WNDPROC WindowProcedure() const noexcept {
			return cls.lpfnWndProc;
		}

		inline HINSTANCE Instance() const noexcept {
			return cls.hInstance;
		}

		inline uint32_t Styles() const noexcept {
			return cls.style;
		}

		inline std::optional<HICON> Icon() const noexcept {
			return cls.hIcon;
		}

		inline std::optional<HICON> SmallIcon() const noexcept {
			return cls.hIconSm;
		}

		inline std::optional<HCURSOR> Cursor() const noexcept {
			return cls.hCursor;
		}

		inline std::optional<HBRUSH> BackgroundBrush() const noexcept {
			return cls.hbrBackground;
		}

		inline std::optional<LPCWSTR> MenuName() const noexcept {
			return cls.lpszMenuName;
		}
	};
}
