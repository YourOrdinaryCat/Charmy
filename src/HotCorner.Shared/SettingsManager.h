#pragma once
#include "MonitorSettings.h"
#include <filesystem>

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
		std::vector<MonitorSettings> Monitors{};

		SettingsManager(const std::filesystem::path& folder) noexcept;

		void Load();
		void Save() const;
	};
}
