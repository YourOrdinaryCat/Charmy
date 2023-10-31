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
using namespace std::string_view_literals;

using SettingsStream = json::EncodedOutputStream<json::UTF16LE<>, json::FileWriteStream>;
using SettingsWriter = json::PrettyWriter<SettingsStream, json::UTF16LE<>>;

namespace winrt::HotCorner::Settings {
	SettingsManager::SettingsManager(const std::filesystem::path& folder) noexcept :
		m_path(folder / SettingsFileName) { }

	void SettingsManager::Save() const {
		OutputDebugString(m_path.c_str());
		const auto hFile = CreateFileFromAppW(
			m_path.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if (hFile != INVALID_HANDLE_VALUE) {
			int fileDescriptor = _open_osfhandle(reinterpret_cast<intptr_t>(hFile), _O_RDWR);
			if (fileDescriptor != -1) {
				const auto file = _fdopen(fileDescriptor, "r+");

				if (file != NULL) {
					SaveTo(file);
					fclose(file);
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
				CloseHandle(hFile);
			}
		}
		else {
			//TODO: Handle failure
			OutputDebugString(L"Failed to find file\n");
		}
	}

	void SettingsManager::SaveTo(FILE* file) const {
		static constexpr auto initialComment = L"//TODO: JSON Schema\n"sv;
		static constexpr auto schemaKey = L"$schema"sv;
		static constexpr auto schema = L"TODO"sv;

		std::array<char, 1024> buffer{};
		json::FileWriteStream filestream(file, buffer.data(), buffer.size());

		SettingsStream out(filestream, true);

		for (const wchar_t c : initialComment) {
			out.Put(c);
		}

		SettingsWriter writer(out);
		writer.SetIndent(' ', 2);

		writer.StartObject();
		writer.Key(schemaKey.data(), static_cast<json::SizeType>(schemaKey.length()));
		writer.String(schema.data(), static_cast<json::SizeType>(schema.length()));

		writer.Key(L"monitors"sv.data(), 8);
		writer.StartArray();

		writer.StartObject();
		MonitorSettings ms{};
		ms.Serialize(writer);
		writer.EndObject();

		writer.EndArray();

		writer.EndObject();

		writer.Flush();
	}
}
