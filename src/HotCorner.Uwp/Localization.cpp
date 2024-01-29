#include "pch.h"
#include "Localization.h"

namespace winrt::HotCorner::Uwp {
	using Windows::ApplicationModel::Resources::ResourceLoader;

	const ResourceLoader& StringLoader() {
		static const auto m_loader = ResourceLoader::GetForViewIndependentUse();
		return m_loader;
	}
}
