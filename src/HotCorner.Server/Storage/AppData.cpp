#include "pch.h"
#include "AppData.h"

#include <appmodel.h>
#include <ShlObj_core.h>
#include <wil/resource.h>

namespace winrt::HotCorner::Server::AppData {
	bool IsPackaged() noexcept {
		UINT32 length = 0;
		const LONG result = GetCurrentPackageFullName(&length, nullptr);

		return result != APPMODEL_ERROR_NO_PACKAGE;
	}

	std::filesystem::path Roaming() {
		wil::unique_cotaskmem_string path{};
		const auto hr = SHGetKnownFolderPath(
			FOLDERID_RoamingAppData,
			KF_FLAG_FORCE_APP_DATA_REDIRECTION,
			nullptr,
			path.put()
		);

		if (hr == S_OK) {
			return path.get();
		}

		throw_hresult(hr);
	}
}
