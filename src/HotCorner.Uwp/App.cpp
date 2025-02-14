#include "pch.h"
#include "App.h"
#include "App.g.cpp"

#include "Controls/EvenStackPanel.h"
#include "Controls/WrapPanel.h"
#include "Views/MainPage.h"
#include <AppSettings.h>
#include <Logging.h>
#include <roerrorapi.h>
#include <Unicode.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wama = winrt::Windows::ApplicationModel::Activation;
namespace wamc = winrt::Windows::ApplicationModel::Core;
namespace wf = winrt::Windows::Foundation;
namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wux = winrt::Windows::UI::Xaml;

namespace winrt::HotCorner::Uwp::implementation {
	// 4:3 aspect ratio <3
	static constexpr wf::Size MainViewSize{ 500, 375 };

	void App::InitializeComponent() {
		Controls::implementation::EvenStackPanel::EnsureProperties();
		Controls::implementation::WrapPanel::EnsureProperties();
	}

	void App::OnLaunched(const wama::LaunchActivatedEventArgs&) const {
		const auto view = wuvm::ApplicationView::GetForCurrentView();
		view.SetPreferredMinSize(MainViewSize);

		const auto window = wux::Window::Current();

		if (!window.Content()) {
			// Initialize these only when creating the initial view
			Logging::Start(L"Settings", SettingsFolder.Path().c_str());

			if (!AppSettings().Load()) {
				// Only resize the view if the settings aren't there - in practical
				// terms, this means it will only be resized on first startup
				view.TryResizeView(MainViewSize);
			}

			Logging::FileSink()->set_level(AppSettings().LogVerbosity);
			window.Content(Views::MainPage());
		}

		window.Activate();
	}

	extern "C" int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
		winrt::init_apartment();
		wamc::CoreApplication::UnhandledErrorDetected([](const IInspectable&, const wamc::UnhandledErrorDetectedEventArgs& e) {
			try {
				e.UnhandledError().Propagate();
			}
			catch (const winrt::hresult_error& err) {
				const auto hr = err.code();
				const auto extra = err.try_as<IRestrictedErrorInfo>().get();

				if (extra) {
					const auto errorSet = SetRestrictedErrorInfo(extra);
					if (errorSet == S_OK) {
						RoFailFastWithErrorContext(hr.value);
					}
				}

				const auto msg = err.message();

				spdlog::critical("Unhandled exception detected - the process will now terminate");
				spdlog::critical("HRESULT: {}", hr.value);
				spdlog::critical("Message: {}", ToMultiByte(msg.data()));
			}
		});

		wux::Application::Start([](auto&&) {
			winrt::make<App>();
		});
		return 0;
	}
}
