#include "pch.h"
#include "TrayIcon.h"
#include "../Resources.h"

#include "../Undocumented/UxTheme.h"

namespace winrt::HotCorner::Server {
	//TODO: Code signing, so that we can identify the icon by its GUID
	TrayIcon::TrayIcon(HINSTANCE instance, const guid& id) noexcept :
		WindowBase(instance, winrt::to_hstring(id), L"NotifyIconHost"),
		m_id(id),
		m_data{
			.cbSize = sizeof(m_data),
			.hWnd = m_window,
			.uID = id.Data1,
			.uFlags = NIF_MESSAGE,
			.uCallbackMessage = TrayIconCallback,
			.uVersion = NOTIFYICON_VERSION_4
		},
		m_taskbarCreated(RegisterMessage(L"TaskbarCreated"))
	{
		Shell_NotifyIcon(NIM_DELETE, &m_data);
	}

	TrayIcon::~TrayIcon() noexcept {
		Hide();
	}

	void TrayIcon::UpdateToolTip(std::wstring_view text) noexcept {
		wcsncpy_s(m_data.szTip, text.data(), 128);
		m_data.uFlags |= NIF_SHOWTIP | NIF_TIP;
	}

	void TrayIcon::SetHighContrastIcon(UINT darkIcon, UINT lightIcon) noexcept {
		m_darkHcIcon = MAKEINTRESOURCE(darkIcon);
		m_lightHcIcon = MAKEINTRESOURCE(lightIcon);

		ReloadIcon(m_canAdd);
	}

	void TrayIcon::UpdateIcon(UINT darkModeIcon, UINT lightModeIcon) noexcept {
		m_darkIcon = MAKEINTRESOURCE(darkModeIcon);
		m_lightIcon = MAKEINTRESOURCE(lightModeIcon);

		ReloadIcon(m_canAdd);
	}

	void TrayIcon::Show() noexcept {
		m_canAdd = true;
		if (!m_visible && Shell_NotifyIcon(NIM_ADD, &m_data)) {
			m_visible = true;
			Shell_NotifyIcon(NIM_SETVERSION, &m_data);
		}
	}

	void TrayIcon::Hide() noexcept {
		m_canAdd = false;
		if (m_visible && Shell_NotifyIcon(NIM_DELETE, &m_data)) {
			m_visible = false;
		}
	}

	// https://stackoverflow.com/a/56678483
	static double sRGBToLinear(double colorChannel) noexcept {
		if (colorChannel <= 0.04045) {
			return colorChannel / 12.92;
		}
		else {
			return std::pow(((colorChannel + 0.055) / 1.055), 2.4);
		}
	}

	static double GetColorLuminance(uint8_t r, uint8_t g, uint8_t b) noexcept {
		const double vR = r / 255.0;
		const double vG = g / 255.0;
		const double vB = b / 255.0;

		return (0.2126 * sRGBToLinear(vR) + 0.7152 * sRGBToLinear(vG) + 0.0722 * sRGBToLinear(vB));
	}

	//TODO: Refactor this whole thing
	static bool IsDark(uint8_t r, uint8_t g, uint8_t b) noexcept {
		return GetColorLuminance(r, g, b) <= 0.5;
	}

	void TrayIcon::ReloadIcon(bool callModify) noexcept {
		HIGHCONTRAST info = { .cbSize = sizeof(info) };

		// If High Contrast is enabled, use special icons
		if (SystemParametersInfo(SPI_GETHIGHCONTRAST, 0, &info, 0) && info.dwFlags & HCF_HIGHCONTRASTON) {
			const auto textColor = GetSysColor(COLOR_WINDOWTEXT);
			const bool isDark = IsDark(
				GetRValue(textColor),
				GetGValue(textColor),
				GetBValue(textColor)
			);

			if (isDark) {
				Resources::GetSmallIcon(m_lightHcIcon, m_currentIcon.put());
			}
			else {
				Resources::GetSmallIcon(m_darkHcIcon, m_currentIcon.put());
			}
		}
		else if (Undocumented::GetCurrentShellTheme() == Undocumented::ShellTheme::Dark) {
			Resources::GetSmallIcon(m_darkIcon, m_currentIcon.put());
		}
		else {
			Resources::GetSmallIcon(m_lightIcon, m_currentIcon.put());
		}

		m_data.hIcon = m_currentIcon.get();
		m_data.uFlags |= NIF_ICON;

		if (callModify) {
			Shell_NotifyIcon(NIM_MODIFY, &m_data);
		}
	}

	LRESULT TrayIcon::HandleMessage(
		UINT message,
		WPARAM wParam,
		LPARAM lParam) noexcept
	{
		switch (message) {
		case WM_DPICHANGED:
		case WM_SETTINGCHANGE:
		case WM_THEMECHANGED:
		case WM_DISPLAYCHANGE:
			ReloadIcon(m_canAdd);
			break;

		case TrayIconCallback:
			return HandleTrayMessage(wParam, lParam);

		default:
			if (message == m_taskbarCreated) {
				ReloadIcon(false);
				m_visible = Shell_NotifyIcon(NIM_MODIFY, &m_data);

				if (m_canAdd) {
					Show();
				}

				return 0;
			}
		}
		return WindowBase::HandleMessage(message, wParam, lParam);
	}
}
