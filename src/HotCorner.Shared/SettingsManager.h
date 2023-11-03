#pragma once
#include "MonitorSettings.h"
#include <filesystem>

namespace winrt::HotCorner::Settings {
	class SettingsManager final {
		static constexpr std::wstring_view SchemaKey = L"$schema";
		static constexpr std::wstring_view MonitorsKey = L"monitors";

		const std::filesystem::path m_path;

		void LoadFrom(FILE* file);
		void SaveTo(FILE* file) const;

	public:
		static constexpr std::wstring_view SettingsFileName = L"settings.json";

		std::vector<MonitorSettings> Monitors{};

		SettingsManager(const std::filesystem::path& folder) noexcept;

		void Load();
		void Save() const;
	};
}
