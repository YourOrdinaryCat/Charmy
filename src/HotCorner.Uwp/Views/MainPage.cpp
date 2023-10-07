#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"

namespace wux = winrt::Windows::UI::Xaml;

namespace winrt::HotCorner::Uwp::Views::implementation {
	void MainPage::ClickHandler(const IInspectable&, const wux::RoutedEventArgs&) {
		MyButton().Content(winrt::box_value(L"Clicked"));
	}
}
