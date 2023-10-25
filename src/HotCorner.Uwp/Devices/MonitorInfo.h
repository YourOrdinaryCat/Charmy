#pragma once
#include "Devices/MonitorInfo.g.h"

namespace winrt::HotCorner::Uwp::Devices::implementation {
	struct MonitorInfo : MonitorInfoT<MonitorInfo> {
	private:
		hstring m_id;
		hstring m_name;
		bool m_connected;

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

	public:
		MonitorInfo(const hstring& id, const hstring& name, bool connected) noexcept;

		inline hstring Id() const noexcept {
			return m_id;
		}

		inline hstring DisplayName() const noexcept {
			return m_name;
		}

		inline bool Connected() const noexcept {
			return m_connected;
		}

		inline void DisplayName(const hstring& value) {
			if (m_name != value) {
				m_name = value;
				m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"DisplayName" });
			}
		}

		inline void Connected(const bool value) {
			if (m_connected != value) {
				m_connected = value;
				m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Connected" });
			}
		}

		winrt::event_token PropertyChanged(const Windows::UI::Xaml::Data::PropertyChangedEventHandler& value) {
			return m_propertyChanged.add(value);
		}

		void PropertyChanged(const winrt::event_token& token) {
			m_propertyChanged.remove(token);
		}
	};
}

namespace winrt::HotCorner::Uwp::Devices::factory_implementation {
	struct MonitorInfo : MonitorInfoT<MonitorInfo, implementation::MonitorInfo> {
	};
}
