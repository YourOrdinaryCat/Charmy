#include "pch.h"
#include "SettingsManager.h"
#include <array>
#include <debugapi.h>
#include <fcntl.h>
#include <fileapifromapp.h>
#include <handleapi.h>
#include <io.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <string_view>
#include <vector>
#include <wil/resource.h>

namespace json = rapidjson;
namespace jh = winrt::HotCorner::Json;
using namespace std::string_view_literals;

using SettingsInputStream = json::EncodedInputStream<json::UTF16LE<>, json::FileReadStream>;
using SettingsOutputStream = json::EncodedOutputStream<json::UTF16LE<>, json::FileWriteStream>;
using SettingsWriter = json::PrettyWriter<SettingsOutputStream, json::UTF16LE<>>;

namespace winrt::HotCorner::Settings {
	//TODO: Why would WIL hide this outside of the desktop family partition?
	typedef wil::unique_any<FILE*, decltype(&::fclose), ::fclose> unique_cfile;

	SettingsManager::SettingsManager(const std::filesystem::path& folder) noexcept :
		m_path(folder / SettingsFileName) { }

	static FILE* FileFromHandle(HANDLE handle, int flags, const char* mode) noexcept {
		if (handle != INVALID_HANDLE_VALUE) {
			int fileDescriptor = _open_osfhandle(reinterpret_cast<intptr_t>(handle), flags);
			if (fileDescriptor != -1) {
				const auto file = _fdopen(fileDescriptor, mode);

				if (file != NULL) {
					return file;
				}
				else {
					//TODO: Handle failure
					OutputDebugString(L"Failed to open file\n");
					_close(fileDescriptor);
				}
			}
			else {
				//TODO: Handle failure
				OutputDebugString(L"Failed to create descriptor\n");
				CloseHandle(handle);
			}
		}
		else {
			//TODO: Handle failure
			OutputDebugString(L"Failed to find file\n");
		}

		return nullptr;
	}

	void SettingsManager::Load() {
		const auto hFile = CreateFileFromAppW(
			m_path.c_str(),
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		const unique_cfile file{ FileFromHandle(hFile, _O_RDONLY, "r") };
		if (file.is_valid()) {
			LoadFrom(file.get());
		}
	}

	void SettingsManager::Save() const {
		const auto hFile = CreateFileFromAppW(
			m_path.c_str(),
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		const unique_cfile file{ FileFromHandle(hFile, _O_WRONLY, "w") };
		if (file.is_valid()) {
			SaveTo(file.get());
		}
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
					Monitors.clear();
					for (const auto& elm : member->value.GetArray()) {
						jh::AssertMatch(elm.GetType(), json::kObjectType);

						MonitorSettings setting{};
						setting.Deserialize(elm.GetObj());

						Monitors.push_back(setting);
					}
				}
			}
		}
		else if (result.Code() != json::kParseErrorDocumentEmpty) {
			//TODO: Handle failure
			OutputDebugString(L"Failed to parse document\n");
		}
	}

	void SettingsManager::SaveTo(FILE* file) const {
		static constexpr auto initialComment = L"//TODO: JSON Schema\n"sv;
		static constexpr auto schema = L"TODO"sv;

		std::array<char, 1024> buffer{};
		json::FileWriteStream filestream(file, buffer.data(), buffer.size());

		SettingsOutputStream out(filestream, true);

		for (const wchar_t c : initialComment) {
			out.Put(c);
		}

		SettingsWriter writer(out);
		writer.SetIndent(' ', 2);

		writer.StartObject();
		jh::KeyValuePair(writer, SchemaKey, schema);
		jh::KeyValuePair(writer, TrackingEnabledKey, TrackingEnabled);
		jh::KeyValuePair(writer, TrayIconEnabledKey, TrayIconEnabled);

		jh::Key(writer, MonitorsKey);

		writer.StartArray();
		for (const auto& setting : Monitors) {
			writer.StartObject();
			setting.Serialize(writer);
			writer.EndObject();
		}
		writer.EndArray();

		writer.EndObject();
		writer.Flush();
	}
}
