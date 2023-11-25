#pragma once
#include <shellapi.h>
#include "WindowBase.h"
#include <wil/resource.h>

namespace winrt::HotCorner::Server {
	/**
	 * @brief A light wrapper for a Win32 notification icon.
	*/
	class TrayIcon : public WindowBase<TrayIcon> {
		friend class WindowBase<TrayIcon>;

		static constexpr uint32_t TrayIconCallback = 0xCA7;

		const guid m_id;
		NOTIFYICONDATAW m_data;

		const std::optional<UINT> m_taskbarCreated;

		bool m_canAdd = false;
		bool m_visible = false;

		const wchar_t* m_darkIcon;
		const wchar_t* m_lightIcon;

		wil::unique_hicon m_currentIcon;
		void ReloadIcon(bool callModify) noexcept;

	protected:
		LRESULT HandleMessage(
			UINT message,
			WPARAM wParam,
			LPARAM lParam) noexcept override;

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
		 * @brief Updates the icon shown to the user.
		 *
		 * @param darkModeIcon The resource ID of a version of the icon for use in dark mode.
		 * @param lightModeIcon The resource ID of a version of the icon for use in light mode.
		*/
		void UpdateIcon(UINT darkModeIcon, UINT lightModeIcon) noexcept;

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
