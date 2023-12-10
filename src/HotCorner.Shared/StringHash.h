#pragma once
#include <functional>
#include <string_view>

namespace winrt::HotCorner {
	template<typename CharT>
	struct basic_string_hash {
		using string_type = std::basic_string<CharT>;
		using string_view_type = std::basic_string_view<CharT>;

		using string_hash_type = std::hash<string_type>;
		using string_view_hash_type = std::hash<string_view_type>;
		using is_transparent = void;

		[[nodiscard]] std::size_t operator()(const CharT* str) const {
			return string_view_hash_type{}(str);
		}
		[[nodiscard]] std::size_t operator()(string_view_type str) const {
			return string_view_hash_type{}(str);
		}
		[[nodiscard]] std::size_t operator()(const string_type& str) const {
			return string_hash_type{}(str);
		}
	};

	using string_hash = basic_string_hash<char>;
#ifdef __cpp_lib_char8_t
	using u8string_hash = basic_string_hash<char8_t>;
#endif // defined(__cpp_lib_char8_t)
	using u16string_hash = basic_string_hash<char16_t>;
	using u32string_hash = basic_string_hash<char32_t>;
	using wstring_hash = basic_string_hash<wchar_t>;
}
