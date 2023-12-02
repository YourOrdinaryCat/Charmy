#pragma once
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::HotCorner::Uwp::Devices {
	namespace wde = Windows::Devices::Enumeration;
	namespace wf = Windows::Foundation;
	namespace wfc = Windows::Foundation::Collections;

	template<typename T>
	concept DeviceInfo = requires(T device, wde::DeviceInformation info, wde::DeviceInformationUpdate update) {
		{ T(info) } -> std::same_as<T>;
		{ device.Id() } -> std::same_as<hstring>;
		{ device.Refresh(info) } -> std::same_as<void>;
		{ device.Refresh(update) } -> std::same_as<void>;
		{ device.Disconnect(update) } -> std::same_as<bool>;
	};

	/**
	 * @brief A watcher based on Windows' DeviceWatcher.
	*/
	template<typename Device, DeviceInfo Info>
	class Watcher {
		const wde::DeviceWatcher m_watcher;
		const wfc::IObservableVector<Info> m_connected{ single_threaded_observable_vector<Info>() };

		const event_token m_addToken;
		const event_token m_removeToken;
		const event_token m_updateToken;
		const event_token m_stoppedToken;

		// Caller might expect updates to be dispatched to the context
		// under which watching started
		apartment_context m_startContext{ nullptr };
		bool m_started = false;

		/**
		 * @brief Gets the index of a monitor in m_monitors based on the provided monitor ID.
		 *
		 * @returns The index if the monitor was found, nullopt otherwise.
		*/
		std::optional<uint32_t> TryGetDeviceIndex(const hstring& id) const noexcept {
			const auto device = std::find_if(m_connected.begin(), m_connected.end(), [&id](const Info& info)
				{
					return info.Id() == id;
				}
			);

			if (device == m_connected.end()) {
				return std::nullopt;
			}

			return static_cast<uint32_t>(std::distance(m_connected.begin(), device));
		}

		fire_and_forget OnDeviceAdded(const wde::DeviceWatcher&, const wde::DeviceInformation device) {
			const auto index = TryGetDeviceIndex(device.Id());
			if (!index.has_value()) {
				Info toAdd{ device };

				co_await m_startContext;
				m_connected.Append(toAdd);
			}
			else {
				const auto info = m_connected.GetAt(*index);

				co_await m_startContext;
				info.Refresh(device);
			}
		}

		fire_and_forget OnDeviceRemoved(const wde::DeviceWatcher&, const wde::DeviceInformationUpdate& update) {
			const auto id = update.Id();

			if (const auto index = TryGetDeviceIndex(id)) {
				const auto info = m_connected.GetAt(*index);

				co_await m_startContext;
				const bool remove = info.Disconnect(update);
				if (remove) {
					m_connected.RemoveAt(*index);
				}
			}
		}

		fire_and_forget OnDeviceUpdated(const wde::DeviceWatcher&, const wde::DeviceInformationUpdate update) {
			const auto id = update.Id();

			if (const auto index = TryGetDeviceIndex(id)) {
				const auto info = m_connected.GetAt(*index);

				co_await m_startContext;
				info.Refresh(update);
			}
		}

		void OnDeviceEnumerationStopped(const wde::DeviceWatcher&, const wf::IInspectable&) {
			if (m_watcher.Status() != wde::DeviceWatcherStatus::Stopped) {
				OutputDebugString(L"Device enumeration has stopped unexpectedly\n");
			}
		}

	public:
		Watcher() :
			m_watcher(wde::DeviceInformation::CreateWatcher(
				Device::GetDeviceSelector()
			)),
			m_addToken(m_watcher.Added({ this, &Watcher::OnDeviceAdded })),
			m_removeToken(m_watcher.Removed({ this, &Watcher::OnDeviceRemoved })),
			m_updateToken(m_watcher.Updated({ this, &Watcher::OnDeviceUpdated })),
			m_stoppedToken(m_watcher.Stopped({ this, &Watcher::OnDeviceEnumerationStopped }))
		{ }

		void Start() {
			if (!m_started) {
				m_started = true;
				m_startContext = {};

				m_watcher.Start();
			}
		}

		const wfc::IObservableVector<Info>& ConnectedDevices() const noexcept {
			return m_connected;
		}
	};
}
