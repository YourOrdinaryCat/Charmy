#include "pch.h"
#include "SettingsManager.h"
#include <array>
#include <debugapi.h>
#include <io.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

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
		const auto err = _wfopen_s(&file, m_path.c_str(), L"r");

		if (err == 0) {
			LoadFrom(file);
			fclose(file);
		}
		else {
			OutputDebugString(L"Unable to open save file\n");
		}

		return err == 0;
	}

	bool SettingsManager::Save() const {
		FILE* file = nullptr;
		const auto err = _wfopen_s(&file, m_path.c_str(), L"w");

		if (err == 0) {
			SaveTo(file);
			fclose(file);
		}
		else {
			OutputDebugString(L"Unable to create or open save file\n");
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
			// Remove the schema key to avoid a false unknown key warning
			doc.RemoveMember(jh::GetValue(SchemaKey));

			for (auto member = doc.MemberBegin(); member != doc.MemberEnd(); ++member) {
				const auto key = jh::GetStringView(member->name);
				if (key == TrackingEnabledKey) {
					jh::ReadValue(member->value, TrackingEnabled);
				}
				else if (key == TrayIconEnabledKey) {
					jh::ReadValue(member->value, TrayIconEnabled);
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
		else if (result.Code() == json::kParseErrorDocumentEmpty) {
			DefaultSettings = {};
			Monitors.clear();
		}
		else {
			//TODO: Handle failure
			OutputDebugString(L"Failed to parse document\n");
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
