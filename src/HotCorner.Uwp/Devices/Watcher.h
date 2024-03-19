#pragma once
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <EnumFlags.h>

namespace winrt::HotCorner::Uwp::Devices {
	namespace wde = Windows::Devices::Enumeration;
	namespace wf = Windows::Foundation;

	/**
	 * @brief This concept represents a minimal interface required to uniquely
	 *        identify and update a device's information through Windows APIs.
	*/
	template<typename T>
	concept DeviceInfo = requires(T device, wde::DeviceInformation info, wde::DeviceInformationUpdate update) {
		{ T::FromDeviceAsync(info) } -> std::same_as<wf::IAsyncOperation<T>>;
		{ device.Id() } -> std::same_as<hstring>;
		{ device.RefreshAsync(info) } -> std::same_as<wf::IAsyncAction>;
		{ device.RefreshAsync(update) } -> std::same_as<wf::IAsyncAction>;
	};

	/**
	 * @brief Defines a set of values that identify DeviceWacher events.
	*/
	enum class DeviceWatcherEvent : uint32_t {
		None = 0,
		Add = 1,
		Remove = 2
	};
	APPLY_FLAG_OPERATORS_TO_ENUM(DeviceWatcherEvent);

	/**
	 * @brief A watcher based on Windows' DeviceWatcher.
	*/
	template<DeviceInfo Info>
	struct Watcher {
		using DeviceVectorT = Windows::Foundation::Collections::IObservableVector<Info>;
		using DeviceVectorChangedArgsT = Windows::Foundation::Collections::IVectorChangedEventArgs;

		/**
		 * @brief Gets the index of a device based on the provided ID.
		 *
		 * @returns The index if the device was found, nullopt otherwise.
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

	private:
		const wde::DeviceWatcher m_watcher;
		const DeviceVectorT m_connected{ single_threaded_observable_vector<Info>() };

		const event_token m_addToken;
		const event_token m_removeToken;
		const event_token m_updateToken;
		const event_token m_stoppedToken;

		// Caller might expect updates to be dispatched to the context
		// under which watching started
		apartment_context m_startContext{ nullptr };
		bool m_running = false;

		bool m_clearRequested = false;
		bool m_restartRequested = false;

		const DeviceWatcherEvent m_handledEvents;

		fire_and_forget OnDeviceAdded(const wde::DeviceWatcher&, const wde::DeviceInformation device) {
			if (const auto index = TryGetDeviceIndex(device.Id())) {
				const auto info = m_connected.GetAt(*index);

				co_await m_startContext;
				co_await info.RefreshAsync(device);
			}
			else if (HasFlag(m_handledEvents, DeviceWatcherEvent::Add)) {
				const Info toAdd = co_await Info::FromDeviceAsync(device);

				if (AddingDeviceOverride(toAdd)) {
					co_await m_startContext;
					m_connected.Append(toAdd);
				}
			}
		}

		fire_and_forget OnDeviceRemoved(const wde::DeviceWatcher&, const wde::DeviceInformationUpdate& update) {
			if (HasFlag(m_handledEvents, DeviceWatcherEvent::Remove)) {
				if (const auto index = TryGetDeviceIndex(update.Id())) {
					const auto device = m_connected.GetAt(*index);
					if (RemovingDeviceOverride(device)) {
						co_await m_startContext;
						m_connected.RemoveAt(*index);
					}
				}
			}
		}

		fire_and_forget OnDeviceUpdated(const wde::DeviceWatcher&, const wde::DeviceInformationUpdate update) {
			const auto id = update.Id();

			if (const auto index = TryGetDeviceIndex(id)) {
				const auto info = m_connected.GetAt(*index);

				co_await m_startContext;
				co_await info.RefreshAsync(update);
			}
		}

		fire_and_forget OnDeviceEnumerationStopped(const wde::DeviceWatcher&, const wf::IInspectable&) {
			if (m_clearRequested) {
				m_clearRequested = false;

				co_await m_startContext;
				m_connected.Clear();
			}

			if (m_watcher.Status() != wde::DeviceWatcherStatus::Stopped) {
				OutputDebugString(L"Device enumeration has stopped unexpectedly\n");
			}

			if (m_restartRequested) {
				m_restartRequested = false;
				m_watcher.Start();
			}
			else {
				m_running = false;
			}
		}

	protected:
		/**
		 * @brief When overriden in a derived class, it can be used to
		 *        determine whether the provided device should be added to the
		 *        connected device collection.
		 *
		 * @returns Whether the provided device should be added to the
		 *          connected device collection. If not overriden, simply
		 *          returns true.
		*/
		virtual bool AddingDeviceOverride(const Info&) const {
			return true;
		}

		/**
		 * @brief When overriden in a derived class, it can be used to
		 *        determine whether the provided device should be removed from
		 *        the connected device collection.
		 *
		 * @returns Whether the provided device should be removed from the
		 *          connected device collection. If not overriden, simply
		 *          returns true.
		*/
		virtual bool RemovingDeviceOverride(const Info&) const {
			return true;
		}

	public:
		Watcher(
			const hstring& selector,
			const DeviceWatcherEvent handledEvents = DeviceWatcherEvent::Add | DeviceWatcherEvent::Remove
		) :
			m_watcher(wde::DeviceInformation::CreateWatcher(selector)),
			m_handledEvents(handledEvents),
			m_addToken(m_watcher.Added({ this, &Watcher::OnDeviceAdded })),
			m_removeToken(m_watcher.Removed({ this, &Watcher::OnDeviceRemoved })),
			m_updateToken(m_watcher.Updated({ this, &Watcher::OnDeviceUpdated })),
			m_stoppedToken(m_watcher.Stopped({ this, &Watcher::OnDeviceEnumerationStopped }))
		{ }

		~Watcher() {
			m_watcher.Added(m_addToken);
			m_watcher.Removed(m_removeToken);
			m_watcher.Updated(m_updateToken);
			m_watcher.Stopped(m_stoppedToken);

			Stop(false);
		}

		void Start() {
			if (!m_running) {
				m_startContext = {};
				m_watcher.Start();

				m_running = true;
			}
		}

		void Stop(const bool clear) {
			if (m_running) {
				m_clearRequested = clear;
				m_watcher.Stop();
			}
		}

		void Restart(const bool clear) {
			if (m_running) {
				m_clearRequested = clear;
				m_restartRequested = true;
				m_watcher.Stop();
			}
		}

		DeviceVectorT ConnectedDevices() const noexcept {
			return m_connected;
		}
	};
}
