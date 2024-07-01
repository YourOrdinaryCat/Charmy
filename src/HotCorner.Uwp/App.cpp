#include "pch.h"
#include "App.h"
#include "Views/MainPage.h"
#include <AppSettings.h>
#include <Logging.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace wama = winrt::Windows::ApplicationModel::Activation;
namespace wamc = winrt::Windows::ApplicationModel::Core;
namespace wf = winrt::Windows::Foundation;
namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wux = winrt::Windows::UI::Xaml;

namespace winrt::HotCorner::Uwp::implementation {
	// 4:3 aspect ratio <3
	static constexpr wf::Size MainViewSize{ 500, 375 };

	App::App() {
		UnhandledException([this](const IInspectable&, const wux::UnhandledExceptionEventArgs& e)
			{
				const auto hr = e.Exception();
				const auto errorMessage = e.Message();

				spdlog::critical("Unhandled exception detected - the process will now terminate");
				spdlog::critical("HRESULT: {}", hr.value);
			}
		);
	}

	void App::OnLaunched(const wama::LaunchActivatedEventArgs&) const {
		const auto view = wuvm::ApplicationView::GetForCurrentView();
		view.SetPreferredMinSize(MainViewSize);

		const auto window = wux::Window::Current();

		if (!window.Content()) {
			// Initialize these only when creating the initial view
			Logging::Start(L"Settings", SettingsPath);

			if (!AppSettings().Load()) {
				// Only resize the view if the settings aren't there - in practical
				// terms, this means it will only be resized on first startup
				view.TryResizeView(MainViewSize);
			}

			window.Content(Views::MainPage());
		}

		window.Activate();
	}
}
