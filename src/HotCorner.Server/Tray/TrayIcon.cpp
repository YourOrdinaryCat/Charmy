#include "pch.h"
#include "TrayIcon.h"

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
		}
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

	void TrayIcon::UpdateIcon(HICON, HICON lightIcon) noexcept {
		//TODO: Handle shell light/dark mode, and high contrast
		m_data.hIcon = lightIcon;
		m_data.uFlags |= NIF_ICON;
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

	LRESULT TrayIcon::HandleMessage(
		UINT message,
		WPARAM wParam,
		LPARAM lParam) noexcept
	{
		return WindowBase::HandleMessage(message, wParam, lParam);
	}
}
