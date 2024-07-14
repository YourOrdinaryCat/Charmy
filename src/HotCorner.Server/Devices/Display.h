#pragma once
#include <array>
#include <compare>
#include <Windows.h>

namespace winrt::HotCorner::Server::Devices {
	static constexpr DWORD DisplayAttachedToDesktop = DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;

	using DisplayName = std::array<WCHAR, 32>;
	using DisplayId = std::array<WCHAR, 128>;

	/**
	 * @brief Gets a copy of the provided display's device name. This is necessary when
	 *        using EnumDisplayDevices with a device name.
	*/
	static inline DisplayName GetDisplayName(const DISPLAY_DEVICE& display)
		noexcept
	{
		std::array<WCHAR, 32> name{};
		wcscpy_s(name.data(), name.size(), display.DeviceName);

		return name;
	}

	/**
	 * @brief Sets the provided display's device Id. This calls EnumDisplayDevices with
	 *        the provided device name and writes the data to the provided
	 *        DISPLAY_DEVICE.
	*/
	static inline bool TrySetDisplayId(const WCHAR* name, DISPLAY_DEVICE* display)
		noexcept
	{
		return EnumDisplayDevices(name, 0, display, EDD_GET_DEVICE_INTERFACE_NAME);
	}

	/**
	 * @brief Gets the provided display's current settings, writing them to the provided
	 *        DEVMODE structure.
	*/
	static inline bool TryGetDisplaySettings(const WCHAR* name, DEVMODE* dm)
		noexcept
	{
		ZeroMemory(dm, sizeof(DEVMODE));
		dm->dmSize = sizeof(DEVMODE);

		return EnumDisplaySettings(name, ENUM_CURRENT_SETTINGS, dm);
	}

	class Displays;
	/**
	 * @brief An iterator that acts as a wrapper for EnumDisplayDevices. To get an
	 *        instance of it, use the Displays class.
	*/
	class DisplayDeviceIterator final {
		const DWORD m_flags = 0;

		bool m_hasMore = false;
		DWORD m_index = 0;
		DISPLAY_DEVICE m_device = { };

		constexpr DisplayDeviceIterator() noexcept = default;
		inline DisplayDeviceIterator(DWORD flags, DWORD index) noexcept :
			m_flags(flags), m_index(index)
		{
			++(*this);
		}

		friend class Displays;

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

		constexpr DISPLAY_DEVICE& operator*() noexcept {
			return m_device;
		}

		constexpr const DISPLAY_DEVICE& operator*() const noexcept {
			return m_device;
		}

		constexpr DISPLAY_DEVICE* operator->() noexcept {
			return &m_device;
		}

		constexpr const DISPLAY_DEVICE* operator->() const noexcept {
			return &m_device;
		}
	};

	/**
	 * @brief Provides access to a DisplayDeviceIterator.
	*/
	class Displays final {
		const DWORD m_flags = 0;
		const DWORD m_index = 0;

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
		constexpr Displays(DWORD flags = 0, DWORD index = 0) noexcept :
			m_flags(flags), m_index(index) { }

		inline DisplayDeviceIterator begin() const noexcept {
			return { m_flags, m_index };
		}

		constexpr DisplayDeviceIterator end() const noexcept {
			return { };
		}
	};
}