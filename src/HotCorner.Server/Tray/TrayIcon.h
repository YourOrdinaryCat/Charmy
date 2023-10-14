#pragma once
#include "WindowBase.h"
#include <Unknwn.h>
#include <shellapi.h>
#include <winrt/base.h>

namespace winrt::HotCorner::Server {
	/**
	 * @brief A light wrapper for a Win32 notification icon.
	*/
	class TrayIcon final : public WindowBase<TrayIcon> {
		friend class WindowBase<TrayIcon>;

		static constexpr uint32_t TrayIconCallback = 0xCA7;

		const guid m_id;
		NOTIFYICONDATAW m_data;

		bool m_canAdd = false;
		bool m_visible = false;

	protected:
		LRESULT HandleMessage(
			UINT message,
			WPARAM wParam,
			LPARAM lParam) noexcept override final;

	public:
		TrayIcon(HINSTANCE instance, const guid id) noexcept;
		~TrayIcon() noexcept;

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
		 * @param darkIcon A dark version of the icon, for use in light mode.
		 * @param lightIcon A light version of the icon, for use in dark mode.
		*/
		void UpdateIcon(HICON darkIcon, HICON lightIcon) noexcept;

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
