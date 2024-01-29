#pragma once
#include <winrt/Windows.ApplicationModel.Resources.h>

namespace winrt::HotCorner::Uwp {
	/**
	 * @brief Gets a singleton instance of a ResourceLoader for view
	 *        independent use.
	*/
	const Windows::ApplicationModel::Resources::ResourceLoader& StringLoader();
}
