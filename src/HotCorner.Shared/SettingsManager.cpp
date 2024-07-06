#include "pch.h"
#include "SettingsManager.h"
#include <array>
#include <io.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <spdlog/spdlog.h>

namespace json = rapidjson;
namespace jh = winrt::HotCorner::Json;

using SettingsInputStream = json::EncodedInputStream<json::UTF16LE<>, json::FileReadStream>;
using SettingsOutputStream = json::EncodedOutputStream<json::UTF16LE<>, json::FileWriteStream>;
using SettingsWriter = json::PrettyWriter<SettingsOutputStream, json::UTF16LE<>>;

namespace winrt::HotCorner::Settings {
	SettingsManager::SettingsManager(const std::filesystem::path& folder) noexcept :
		m_path(folder / SettingsFileName) { }

	bool SettingsManager::Load() {
		FILE* file = nullptr;
		const auto err = _wfopen_s(&file, m_path.c_str(), L"rbS");

		if (err == 0) {
			LoadFrom(file);
			fclose(file);
		}
		else {
			spdlog::info("Unable to open save file. Error: {}", err);
		}

		return err == 0;
	}

	bool SettingsManager::Save() const {
		FILE* file = nullptr;
		const auto err = _wfopen_s(&file, m_path.c_str(), L"wbS");

		if (err == 0) {
			SaveTo(file);
			fclose(file);
		}
		else {
			spdlog::error("Unable to create or open save file. Error: {}", err);
		}

		return err == 0;
	}

	void SettingsManager::LoadFrom(FILE* file) {
		std::array<char, 1024> buffer{};
		json::FileReadStream filestream(file, buffer.data(), buffer.size());
		SettingsInputStream in(filestream);

		json::GenericDocument<json::UTF16LE<>> doc{};
		const json::ParseResult result = doc.ParseStream<json::kParseCommentsFlag, json::UTF16LE<>>(in);

		if (result) {
			try {
				// Remove the schema key to avoid a false unknown key warning
				doc.RemoveMember(jh::GetValue(SchemaKey));

				for (auto member = doc.MemberBegin(); member != doc.MemberEnd(); ++member) {
					const auto key = jh::GetStringView(member->name);
					if (key == TrackingEnabledKey) {
						jh::ReadValue(member->value, key, TrackingEnabled);
					}
					else if (key == TrayIconEnabledKey) {
						jh::ReadValue(member->value, key, TrayIconEnabled);
					}
					else if (key == LogVerbosityKey) {
						jh::ReadMappedValue(member->value, LogVerbosityMapping, key, LogVerbosity);
					}
					else if (key == MonitorsKey) {
						DefaultSettings = {};
						Monitors.clear();

						const auto val = member->value.GetObj();
						for (auto monitor = val.MemberBegin(); monitor != val.MemberEnd(); ++monitor) {
							const auto id = jh::GetStringView(monitor->name);
							if (!id.empty()) {
								Monitors.emplace(id, monitor->value);
							}
							else {
								DefaultSettings.Deserialize(monitor->value);
							}
						}
					}
				}
			}
			catch (std::logic_error err) {
				spdlog::error("Something went wrong while trying to parse document. Error: {}", err.what());
			}
		}
		else if (result.Code() == json::kParseErrorDocumentEmpty) {
			DefaultSettings = {};
			Monitors.clear();
		}
		else {
			spdlog::warn("Failed to parse document");
		}
	}

	void SettingsManager::SaveTo(FILE* file) const {
		std::array<char, 1024> buffer{};
		json::FileWriteStream filestream(file, buffer.data(), buffer.size());

		SettingsOutputStream out(filestream, true);

		for (const wchar_t c : InitialComment) {
			out.Put(c);
		}

		SettingsWriter writer(out);
		writer.SetIndent(' ', 2);

		writer.StartObject();
		jh::KeyValuePair(writer, SchemaKey, Schema);
		jh::KeyValuePair(writer, TrackingEnabledKey, TrackingEnabled);
		jh::KeyValuePair(writer, TrayIconEnabledKey, TrayIconEnabled);
		jh::MappedKVP(writer, LogVerbosityMapping, LogVerbosityKey, LogVerbosity);

		jh::Key(writer, MonitorsKey);
		writer.StartObject();

		jh::Key(writer, L"");

		writer.StartObject();
		DefaultSettings.Serialize(writer);
		writer.EndObject();

		for (auto&& setting : Monitors) {
			jh::Key(writer, setting.first);

			writer.StartObject();
			setting.second.Serialize(writer);
			writer.EndObject();
		}
		writer.EndObject();

		writer.EndObject();
		writer.Flush();
	}
}
