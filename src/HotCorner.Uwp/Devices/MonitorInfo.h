#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.UI.Xaml.Data.h>

#include "Devices/MonitorInfo.g.h"

namespace winrt::HotCorner::Uwp::Devices::implementation {
	struct MonitorInfo : MonitorInfoT<MonitorInfo> {
	private:
		const hstring m_id;
		hstring m_name;

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
		Windows::Foundation::IAsyncAction RefreshAsync(const hstring& id);

	public:
		MonitorInfo(const hstring& id, const hstring& name) noexcept;

		static Windows::Foundation::IAsyncOperation<Devices::MonitorInfo> FromDeviceAsync(const Windows::Devices::Enumeration::DeviceInformation& device);

		inline Windows::Foundation::IAsyncAction RefreshAsync(const Windows::Devices::Enumeration::DeviceInformationUpdate& update) {
			return RefreshAsync(update.Id());
		}

		inline Windows::Foundation::IAsyncAction RefreshAsync(const Windows::Devices::Enumeration::DeviceInformation& info) {
			return RefreshAsync(info.Id());
		}

		inline hstring Id() const noexcept {
			return m_id;
		}

		inline hstring DisplayName() const noexcept {
			return m_name;
		}

		inline void DisplayName(const hstring& value) {
			if (m_name != value) {
				m_name = value;
				m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"DisplayName" });
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
