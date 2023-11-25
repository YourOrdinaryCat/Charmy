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

	void TrayIcon::ReloadIcon(bool callModify) noexcept {
		if (Undocumented::GetCurrentShellTheme() == Undocumented::ShellTheme::Dark) {
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
