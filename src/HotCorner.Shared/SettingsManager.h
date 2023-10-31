#pragma once
#include "MonitorSettings.h"
#include <filesystem>

namespace winrt::HotCorner::Settings {
	class SettingsManager final {
		const std::filesystem::path m_path;
		void SaveTo(FILE* file) const;

	public:
		static constexpr std::wstring_view SettingsFileName = L"settings.json";

		std::vector<MonitorSettings> Monitors{};

		SettingsManager(const std::filesystem::path& folder) noexcept;

		void Save() const;
	};
}
