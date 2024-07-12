#pragma once
#include "Logging.h"
#include "Unicode.h"
#include <array>
#include <format>
#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson { typedef ::std::size_t SizeType; }
#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <string_view>

namespace winrt::HotCorner::Json {
	using type = rapidjson::Type;
	using value_t = rapidjson::GenericValue<rapidjson::UTF16LE<>>;

	template<size_t Count>
	using type_mapping = std::array<std::wstring_view, Count>;

	static constexpr std::array<std::string_view, 7> JsonTypesMapping = {
		"null",
		"bool",
		"bool",
		"object",
		"array",
		"string",
		"number"
	};

	constexpr bool AreEqual(type left, type right) noexcept {
		return left == right ||
			(left == type::kFalseType && right == type::kTrueType) ||
			(left == type::kTrueType && right == type::kFalseType);
	}

	/**
	 * @brief Asserts whether the provided types match - if they don't, throws an
	 *        exception.
	*/
	inline void AssertMatch(type expected, type toMatch) {
		if (!AreEqual(expected, toMatch)) [[unlikely]] {
			throw std::logic_error{
				std::format(
					"Expected {} but found {}",
					JsonTypesMapping.at(expected),
					JsonTypesMapping.at(toMatch)
				)
			};
		}
	}

	/**
	 * @brief Asserts whether the provided types match - if they don't, throws an
	 *        exception.
	*/
	inline void AssertMatch(type expected, type toMatch, std::wstring_view key) {
		if (!AreEqual(expected, toMatch)) [[unlikely]] {
			throw std::logic_error{
				std::format(
					"Expected {} but found {} while deserializing '{}'",
					JsonTypesMapping.at(expected),
					JsonTypesMapping.at(toMatch),
					ToMultiByte(key.data())
				)
			};
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
	inline value_t GetValue(std::wstring_view str) noexcept {
		return { str.data(), str.length() };
	}

	inline void ReadValue(const value_t& json, std::wstring_view key, bool& value) {
		AssertMatch(type::kTrueType, json.GetType(), key);
		value = json.GetBool();
	}

	inline void ReadValue(const value_t& json, std::wstring_view key, unsigned int& value) {
		AssertMatch(type::kNumberType, json.GetType(), key);
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
		const type_mapping<MappingSize>& mappings,
		std::wstring_view key,
		T& value)
	{
		AssertMatch(type::kStringType, json.GetType(), key);

		const auto mapped = GetStringView(json);
		const auto found = std::find(mappings.begin(), mappings.end(), mapped);

		if (found != mappings.end()) {
			value = static_cast<T>(std::distance(mappings.begin(), found));
		}
		else {
			throw std::logic_error{
				std::format(
					"Invalid enum string detected ('{}') while deserializing '{}'",
					ToMultiByte(mapped.data()),
					ToMultiByte(key.data())
				)
			};
		}
	}

	template<class Writer>
	inline void Key(Writer& writer, std::wstring_view key) {
		writer.Key(key.data(), key.length());
	}

	template<class Writer>
	inline void String(Writer& writer, std::wstring_view value) {
		writer.String(value.data(), value.length());
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
		const type_mapping<MappingSize>& mappings,
		std::wstring_view key,
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
