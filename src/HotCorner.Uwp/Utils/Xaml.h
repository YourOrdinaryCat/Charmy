#pragma once

#ifdef USE_WINUI3
#define XAML winrt::Microsoft::UI::Xaml
#else
#define XAML winrt::Windows::UI::Xaml
#endif

namespace winrt {
	namespace Microsoft::UI::Xaml {
		namespace Controls {}
	}

	namespace Windows::UI::Xaml {
		namespace Controls {}
	}
}

namespace XamlAliases {
	namespace mux = winrt::Microsoft::UI::Xaml;
	namespace muxc = mux::Controls;

	namespace wux = XAML;
	namespace wuxc = wux::Controls;
}
