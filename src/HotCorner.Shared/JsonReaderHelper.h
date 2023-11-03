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

	inline void AssertMatch(type expected, type toMatch) {
		if (!AreEqual(expected, toMatch)) [[unlikely]] {
			//TODO: Handle failure
		}
	}

	inline std::wstring_view GetStringView(const value_t& json) {
		return { json.GetString(), json.GetStringLength() };
	}

	inline void ReadValue(const value_t& json, bool& value) {
		value = json.GetBool();
	}

	inline void ReadValue(const value_t& json, unsigned int& value) {
		value = json.GetUint();
	}

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
}
