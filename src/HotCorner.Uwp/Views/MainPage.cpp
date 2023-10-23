#include "pch.h"
#include "MainPage.h"
#include "Views/MainPage.g.cpp"

#include "../Server/Lifetime.h"

namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wux = winrt::Windows::UI::Xaml;

namespace winrt::HotCorner::Uwp::Views::implementation {
	void MainPage::OnStartStopButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().TrackHotCorners();
		Lifetime::Current().ShowTrayIcon();
	}

	winrt::fire_and_forget MainPage::OnOKButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().HideTrayIcon();
		Lifetime::Current().StopTracking();

		co_await wuvm::ApplicationView::GetForCurrentView().TryConsolidateAsync();
	}

	winrt::fire_and_forget MainPage::OnCancelButtonClick(const IInspectable&, const wux::RoutedEventArgs&) {
		Lifetime::Current().HideTrayIcon();
		Lifetime::Current().StopTracking();

		co_await wuvm::ApplicationView::GetForCurrentView().TryConsolidateAsync();
	}
}
