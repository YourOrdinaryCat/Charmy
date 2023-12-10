#pragma once
#include "MonitorSettings.h"
#include "StringHash.h"
#include <filesystem>
#include <unordered_map>

namespace winrt::HotCorner::Settings {
	class SettingsManager final {
		static constexpr std::wstring_view SchemaKey = L"$schema";
		static constexpr std::wstring_view TrackingEnabledKey = L"enabled";
		static constexpr std::wstring_view TrayIconEnabledKey = L"show_tray_icon";
		static constexpr std::wstring_view MonitorsKey = L"monitors";

		const std::filesystem::path m_path;

		void LoadFrom(FILE* file);
		void SaveTo(FILE* file) const;

	public:
		static constexpr std::wstring_view SettingsFileName = L"settings.json";

		bool TrackingEnabled = true;
		bool TrayIconEnabled = false;
		std::unordered_map<std::wstring, MonitorSettings, wstring_hash, std::equal_to<>> Monitors{};

		SettingsManager(const std::filesystem::path& folder) noexcept;

		void Load();
		void Save() const;

		inline std::optional<MonitorSettings> TryGetSetting(std::wstring_view id) const {
			if (const auto setting = Monitors.find(id); setting != Monitors.end()) {
				return setting->second;
			}
			return std::nullopt;
		}

		inline MonitorSettings GetSettingOrDefaults(std::wstring_view id) const {
			if (const auto setting = Monitors.find(id); setting != Monitors.end()) {
				return setting->second;
			}
			return Monitors.at(L"");
		}
	};
}
