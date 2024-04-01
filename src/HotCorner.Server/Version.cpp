#include "pch.h"
#include "Version.h"

#include <winrt/Windows.Foundation.Metadata.h>

namespace wfm = winrt::Windows::Foundation::Metadata;

namespace winrt::HotCorner::Server {
	bool IsAtLeast1903() noexcept {
		return wfm::ApiInformation::IsApiContractPresent(
			L"Windows.Foundation.UniversalApiContract", 8
		);
	}
}
