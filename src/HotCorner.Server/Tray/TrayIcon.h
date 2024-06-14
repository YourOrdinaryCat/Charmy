#pragma once
#include "WindowBase.h"
#include <shellapi.h>
#include <wil/resource.h>

namespace winrt::HotCorner::Server {
	/**
	 * @brief A light wrapper for a Win32 notification icon.
	*/
	class TrayIcon : public WindowBase<TrayIcon> {
		friend class WindowBase<TrayIcon>;

		const guid m_id;
		NOTIFYICONDATAW m_data;

		const std::optional<UINT> m_taskbarCreated;

		bool m_canAdd = false;
		bool m_visible = false;

		const wchar_t* m_darkIcon;
		const wchar_t* m_lightIcon;

		const wchar_t* m_darkHcIcon;
		const wchar_t* m_lightHcIcon;

		wil::unique_hicon m_currentIcon;
		void ReloadIcon(bool callModify) noexcept;

	protected:
		/**
		 * @brief The backing window's message identifier.
		*/
		static constexpr uint32_t TrayIconCallback = 0xACA7;

		LRESULT HandleMessage(
			UINT message,
			WPARAM wParam,
			LPARAM lParam) noexcept override;

		/**
		 * @brief A separate message handler for notification messages associated with
		 *        the tray icon. Override this in a derived class to handle those
		 *        messages on your own - by default, all calls to this method are
		 *        forwarded to DefWindowProcW.
		*/
		inline virtual LRESULT HandleTrayMessage(WPARAM wParam, LPARAM lParam) noexcept {
			return DefWindowProc(m_window, TrayIconCallback, wParam, lParam);
		}

	public:
		TrayIcon(HINSTANCE instance, const guid& id) noexcept;
		virtual ~TrayIcon() noexcept;

		/**
		 * @brief Updates the icon's tooltip.
		 *
		 * @param text The new tooltip. Keep in mind that it will be trimmed
		 *             to a maximum of 128 characters.
		*/
		void UpdateToolTip(std::wstring_view text) noexcept;

		/**
		 * @brief Sets the icon to be shown to users using high contrast mode.
		 *
		 * @param darkIcon The resource ID of a version of the icon for use in dark backgrounds.
		 * @param lightIcon The resource ID of a version of the icon for use in light backgrounds.
		 * @param reload Whether to refresh the tray icon immediately - true by default.
		*/
		void SetHighContrastIcon(UINT darkIcon, UINT lightIcon, bool reload = true) noexcept;

		/**
		 * @brief Updates the icon shown to the user.
		 *
		 * @param darkModeIcon The resource ID of a version of the icon for use in dark mode.
		 * @param lightModeIcon The resource ID of a version of the icon for use in light mode.
		 * @param reload Whether to refresh the tray icon immediately - true by default.
		*/
		void UpdateIcon(UINT darkModeIcon, UINT lightModeIcon, bool reload = true) noexcept;

		/**
		 * @brief Whether the icon is being shown in the notification area.
		*/
		inline bool Visible() const noexcept {
			return m_visible;
		}

		/**
		 * @brief Shows the icon in the notification area. Nothing is done
		 *        if the icon is already visible.
		*/
		void Show() noexcept;

		/**
		 * @brief Hides the icon from the notification area. Nothing is done
		 *        if the icon is already hidden.
		 *
		 * @remarks This does not end the underlying window's message loop.
		*/
		void Hide() noexcept;
	};
}
