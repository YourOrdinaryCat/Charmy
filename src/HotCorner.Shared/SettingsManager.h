#pragma once
#include "MonitorSettings.h"
#include "StringHash.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <unordered_map>

namespace winrt::HotCorner::Settings {
	class SettingsManager final {
		static constexpr std::wstring_view InitialComment = L"//TODO: JSON Schema\n";
		static constexpr std::wstring_view Schema = L"TODO";

		static constexpr std::wstring_view SchemaKey = L"$schema";
		static constexpr std::wstring_view TrackingEnabledKey = L"enabled";
		static constexpr std::wstring_view TrayIconEnabledKey = L"show_tray_icon";
		static constexpr std::wstring_view LogVerbosityKey = L"verbosity";
		static constexpr std::wstring_view MonitorsKey = L"monitors";

		static constexpr std::array<std::wstring_view, 7> LogVerbosityMapping = {
			L"trace",
			L"debug",
			L"information",
			L"warning",
			L"error",
			L"critical",
			L"off"
		};

		const std::filesystem::path m_path;

		void LoadFrom(FILE* file);
		void SaveTo(FILE* file) const;

	public:
		static constexpr std::wstring_view SettingsFileName = L"settings.json";

		bool TrackingEnabled = true;
		bool TrayIconEnabled = false;
		spdlog::level::level_enum LogVerbosity = spdlog::level::warn;

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
