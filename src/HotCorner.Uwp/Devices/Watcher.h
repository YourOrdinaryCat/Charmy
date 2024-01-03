#pragma once
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <EnumFlags.h>

namespace winrt::HotCorner::Uwp::Devices {
	namespace wde = Windows::Devices::Enumeration;
	namespace wf = Windows::Foundation;
	namespace wfc = Windows::Foundation::Collections;

	/**
	 * @brief This concept represents a minimal interface required to uniquely
	 *        identify and update a device's information through Windows APIs.
	*/
	template<typename T>
	concept DeviceInfo = requires(T device, wde::DeviceInformation info, wde::DeviceInformationUpdate update) {
		{ T(info) } -> std::same_as<T>;
		{ device.Id() } -> std::same_as<hstring>;
		{ device.Refresh(info) } -> std::same_as<void>;
		{ device.Refresh(update) } -> std::same_as<void>;
	};

	/**
	 * @brief Defines a set of values that specify how the DeviceWatcher.Added
	 *        event should be handled.
	*/
	enum class DeviceAdditionHandlingMode : uint32_t {
		/**
		 * @brief The event is left effectively unhandled.
		*/
		Unhandled = 0,
		/**
		 * @brief Newly connected devices are added to the connected device
		 *        collection.
		*/
		Add = 1,
		/**
		 * @brief If the device is already in the connected device collection,
		 *        its data gets updated.
		*/
		Update = 2
	};
	APPLY_FLAG_OPERATORS_TO_ENUM(DeviceAdditionHandlingMode);

	/**
	 * @brief Defines a set of values that specify how the DeviceWatcher.Removed
	 *        event should be handled.
	*/
	enum class DeviceRemovalHandlingMode : uint32_t {
		/**
		 * @brief The event is left effectively unhandled.
		*/
		Unhandled = 0,
		/**
		 * @brief On disconnection, if the device is in the connected device
		 *        collection, it will be removed.
		*/
		Remove = 1
	};
	APPLY_FLAG_OPERATORS_TO_ENUM(DeviceRemovalHandlingMode);

	/**
	 * @brief A watcher based on Windows' DeviceWatcher.
	*/
	template<DeviceInfo Info>
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
		bool m_running = false;

		const DeviceAdditionHandlingMode m_addMode;
		const DeviceRemovalHandlingMode m_removeMode;

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

		fire_and_forget OnDeviceAdded(const wde::DeviceWatcher&, const wde::DeviceInformation device) {
			if (m_addMode != DeviceAdditionHandlingMode::Unhandled) {
				if (const auto index = TryGetDeviceIndex(device.Id())) {
					if (HasFlag(m_addMode, DeviceAdditionHandlingMode::Update)) {
						const auto info = m_connected.GetAt(*index);

						co_await m_startContext;
						info.Refresh(device);
					}
				}
				else if (HasFlag(m_addMode, DeviceAdditionHandlingMode::Add)) {
					Info toAdd{ device };

					co_await m_startContext;
					m_connected.Append(toAdd);
				}
			}
		}

		fire_and_forget OnDeviceRemoved(const wde::DeviceWatcher&, const wde::DeviceInformationUpdate& update) {
			if (m_removeMode != DeviceRemovalHandlingMode::Unhandled) {
				if (const auto index = TryGetDeviceIndex(update.Id())) {
					co_await m_startContext;
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
			m_running = false;
			if (m_watcher.Status() != wde::DeviceWatcherStatus::Stopped) {
				OutputDebugString(L"Device enumeration has stopped unexpectedly\n");
			}
		}

	public:
		Watcher(
			const hstring& selector,
			const DeviceAdditionHandlingMode addMode = DeviceAdditionHandlingMode::Add | DeviceAdditionHandlingMode::Update,
			const DeviceRemovalHandlingMode removeMode = DeviceRemovalHandlingMode::Remove
		) :
			m_watcher(wde::DeviceInformation::CreateWatcher(selector)),
			m_addMode(addMode),
			m_removeMode(removeMode),
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

			Stop();
		}

		void Start() {
			if (!m_running) {
				m_startContext = {};
				m_watcher.Start();

				m_running = true;
			}
		}

		void Stop() {
			if (m_running) {
				m_watcher.Stop();
			}
		}

		wfc::IObservableVector<Info> ConnectedDevices() const noexcept {
			return m_connected;
		}
	};
}
