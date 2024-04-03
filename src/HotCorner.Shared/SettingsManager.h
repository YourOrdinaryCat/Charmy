#pragma once
#include "MonitorSettings.h"
#include "StringHash.h"
#include <filesystem>
#include <unordered_map>

namespace winrt::HotCorner::Settings {
	class SettingsManager final {
		static constexpr std::wstring_view InitialComment = L"//TODO: JSON Schema\n";
		static constexpr std::wstring_view Schema = L"TODO";

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

		MonitorSettings DefaultSettings{};
		std::unordered_map<std::wstring, MonitorSettings, wstring_hash, std::equal_to<>> Monitors{};

		SettingsManager(const std::filesystem::path& folder) noexcept;

		bool Load();
		bool Save() const;

		inline MonitorSettings& GetSettingOrDefaults(std::wstring_view id) {
			if (auto setting = Monitors.find(id); setting != Monitors.end()) {
				return setting->second;
			}
			return DefaultSettings;
		}
	};
}
