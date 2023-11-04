#pragma once
#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <string_view>

namespace winrt::HotCorner::Json {
	using type = rapidjson::Type;
	using value_t = rapidjson::GenericValue<rapidjson::UTF16LE<>>;

	constexpr bool AreEqual(type left, type right) noexcept {
		return left == right ||
			(left == type::kFalseType && right == type::kTrueType) ||
			(left == type::kTrueType && right == type::kFalseType);
	}

	/**
	 * @brief Asserts whether the provided types match.
	*/
	inline void AssertMatch(type expected, type toMatch) {
		if (!AreEqual(expected, toMatch)) [[unlikely]] {
			//TODO: Handle failure
			__debugbreak();
		}
	}

	/**
	 * @brief Creates a wide string view from the provided JSON value.
	*/
	inline std::wstring_view GetStringView(const value_t& json) {
		AssertMatch(type::kStringType, json.GetType());
		return { json.GetString(), json.GetStringLength() };
	}

	/**
	 * @brief Creates a JSON value from the provided wide string view.
	*/
	inline value_t GetValue(std::wstring_view str) {
		return { str.data(), static_cast<rapidjson::SizeType>(str.length()) };
	}

	inline void ReadValue(const value_t& json, bool& value) {
		value = json.GetBool();
	}

	inline void ReadValue(const value_t& json, unsigned int& value) {
		value = json.GetUint();
	}

	/**
	 * @brief Reads an enum value that is mapped to a set of strings in its JSON
	 *        representation.
	 *
	 * @param json The value to read. Must hold a string.
	 * @param mappings The mappings between the enum values and their string
	 *                 representations. An array index maps to an enum value,
	 *                 and the element at the index is the JSON representation.
	*/
	template<std::size_t MappingSize, class T> requires std::is_enum_v<T>
	inline void ReadMappedValue(
		const value_t& json,
		const std::array<std::wstring_view, MappingSize>& mappings,
		T& value)
	{
		AssertMatch(type::kStringType, json.GetType());

		const auto mapped = GetStringView(json);
		const auto key = std::find(mappings.begin(), mappings.end(), mapped);

		if (key != mappings.end()) {
			value = static_cast<T>(std::distance(mappings.begin(), key));
		}
		else {
			//TODO: Handle failure
		}
	}

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

	/**
	 * @brief Writes an enum value that is mapped to a set of strings in its JSON
	 *        representation.
	 *
	 * @param mappings The mappings between the enum values and their string
	 *                 representations. An array index maps to an enum value,
	 *                 and the element at the index is the JSON representation.
	 * @param value The value to write.
	*/
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
