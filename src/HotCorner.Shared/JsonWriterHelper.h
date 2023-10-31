#pragma once
#include <rapidjson/rapidjson.h>
#include <string_view>

namespace winrt::HotCorner::Json {
	template<class Writer>
	inline void Key(Writer& writer, std::wstring_view key) {
		writer.Key(key.data(), static_cast<rapidjson::SizeType>(key.length()));
	}

	template<class Writer>
	inline void String(Writer& writer, std::wstring_view value) {
		writer.String(value.data(), static_cast<rapidjson::SizeType>(value.length()));
	}

	template<class Writer, std::same_as<bool> T>
	inline void KeyValuePair(Writer& writer, std::wstring_view key, T value) {
		Key(writer, key);
		writer.Bool(value);
	}

	template<class Writer, std::same_as<unsigned int> T>
	inline void KeyValuePair(Writer& writer, std::wstring_view key, T value) {
		Key(writer, key);
		writer.Uint(value);
	}

	template<class Writer, std::size_t MappingSize, class T> requires std::is_enum_v<T>
	inline void MappedKVP(
		Writer& writer,
		std::wstring_view key,
		const std::array<std::wstring_view, MappingSize>& mappings,
		T value)
	{
		const auto index = static_cast<std::size_t>(value);
		if (index < MappingSize) {
			Key(writer, key);
			String(writer, mappings[index]);
		}
	}

	template<class Writer>
	inline void KeyValuePair(Writer& writer, std::wstring_view key, std::wstring_view value) {
		Key(writer, key);
		String(writer, value);
	}
}