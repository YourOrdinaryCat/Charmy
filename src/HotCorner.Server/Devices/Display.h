#pragma once
#include <compare>
#include <Windows.h>

namespace winrt::HotCorner::Server::Devices {
	class EnumDisplays;

	/**
	 * @brief An iterator that acts as a wrapper for EnumDisplayDevices. To get an
	 *        instance of it, use the EnumDisplays class.
	*/
	class DisplayDeviceIterator {
		const DWORD m_flags = 0;

		BOOL m_hasMore = FALSE;
		DWORD m_index = 0;
		DISPLAY_DEVICE m_device = { };

		constexpr DisplayDeviceIterator() noexcept = default;
		inline DisplayDeviceIterator(DWORD flags, DWORD index, BOOL hasMore) noexcept :
			m_flags(flags), m_index(index), m_hasMore(hasMore)
		{
			++(*this);
		}

		friend class EnumDisplays;

	public:
		inline DisplayDeviceIterator& operator ++() noexcept {
			ZeroMemory(&m_device, sizeof(DISPLAY_DEVICE));
			m_device.cb = sizeof(DISPLAY_DEVICE);

			m_hasMore = EnumDisplayDevices(NULL, m_index, &m_device, 0);
			++m_index;

			if (m_flags != 0) {
				while (m_hasMore == TRUE && !(m_device.StateFlags & m_flags)) {
					ZeroMemory(&m_device, sizeof(DISPLAY_DEVICE));
					m_device.cb = sizeof(DISPLAY_DEVICE);

					m_hasMore = EnumDisplayDevices(NULL, m_index, &m_device, 0);
					++m_index;
				}
			}

			return *this;
		}

		constexpr friend bool operator== (
			const DisplayDeviceIterator& a,
			const DisplayDeviceIterator& b) noexcept
		{
			return a.m_hasMore == b.m_hasMore;
		}

		constexpr DISPLAY_DEVICE operator*() const noexcept {
			return m_device;
		}
	};

	/**
	 * @brief Provides access to a DisplayDeviceIterator.
	*/
	class EnumDisplays {
		const DWORD m_index = 0;
		const DWORD m_flags = 0;

	public:
		/**
		 * @brief Creates a new instance of this class, which provides a
		 *        DisplayDeviceIterator.
		 *
		 * @param flags If set, acts as a filter that skips devices whose StateFlags do
		 *              not contain the provided ones. To avoid filtering based on this,
		 *              simply pass 0.
		 * @param index The index to start the search at, where 0 is the first display.
		*/
		constexpr EnumDisplays(DWORD flags = 0, DWORD index = 0) noexcept :
			m_flags(flags), m_index(index) { }

		inline DisplayDeviceIterator begin() const noexcept {
			return { m_flags, m_index, TRUE };
		}

		constexpr DisplayDeviceIterator end() const noexcept {
			return { };
		}
	};
}