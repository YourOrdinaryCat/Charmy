#pragma once
#include "Logging.h"
#include <string>
#include <string_view>
#include <Windows.h>

namespace winrt::HotCorner {
	/**
	 * @brief Converts the provided wide string to a multi byte one, using the specified
	 *        encoding.
	*/
	inline std::string ToMultiByte(std::wstring_view source, UINT codePage = CP_UTF8) {
		const auto sourceSize = static_cast<int>(source.size());

		SetLastError(0);
		const auto finalSize = WideCharToMultiByte(codePage, 0, source.data(), sourceSize, NULL, 0, NULL, NULL);

		if (finalSize) {
			std::string str(finalSize, '\0');

			SetLastError(0);
			if (WideCharToMultiByte(codePage, 0, source.data(), sourceSize, str.data(), finalSize, NULL, NULL)) {
				return str;
			}
		}

		SPDLOG_LAST_ERROR(spdlog::level::err, "Conversion to UTF-8 failed");
		return "";
	}
}