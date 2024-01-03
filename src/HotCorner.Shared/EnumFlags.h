#pragma once
#include <type_traits>

namespace winrt::HotCorner {
	template<typename T>
	concept EnumerationType = std::is_enum_v<T>;

	template<EnumerationType T>
	constexpr bool HasFlag(T toCheck, T flag) noexcept {
		return (toCheck & flag) == flag;
	}
}

/**
 * @brief Adds a set of operators to allow using the provided enum as a set of
 *        flags. This macro can only be used for enumeration types.
*/
#define APPLY_FLAG_OPERATORS_TO_ENUM(TYPE) \
static_assert(std::is_enum_v<TYPE>, "Parameter for the APPLY_FLAG_OPERATORS_TO_ENUM macro must be an enumeration type."); \
constexpr auto operator|(const TYPE left, const TYPE right) noexcept { \
	return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(left) | static_cast<std::underlying_type_t<TYPE>>(right)); \
} \
constexpr auto operator|=(TYPE& left, const TYPE right) noexcept { \
	left = left | right; \
	return left; \
} \
constexpr auto operator&(const TYPE left, const TYPE right) noexcept { \
	return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(left) & static_cast<std::underlying_type_t<TYPE>>(right)); \
} \
constexpr auto operator&=(TYPE& left, const TYPE right) noexcept { \
	left = left & right; \
	return left; \
} \
constexpr auto operator~(const TYPE value) noexcept { \
	return static_cast<TYPE>(~static_cast<std::underlying_type_t<TYPE>>(value)); \
} \
constexpr auto operator^(const TYPE left, const TYPE right) noexcept { \
	return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(left) ^ static_cast<std::underlying_type_t<TYPE>>(right)); \
} \
constexpr auto operator^=(TYPE& left, const TYPE right) noexcept { \
	left = left ^ right; \
	return left; \
}