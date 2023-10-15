#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"

#include "../Server/Lifetime.h"

namespace wux = winrt::Windows::UI::Xaml;

namespace winrt::HotCorner::Uwp::Views::implementation {
	void MainPage::StartHandler(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().TrackHotCorners();
		Lifetime::Current().ShowTrayIcon();
	}

	void MainPage::StopHandler(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().HideTrayIcon();
		Lifetime::Current().StopTracking();
	}
}
