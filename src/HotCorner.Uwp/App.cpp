#include "pch.h"
#include "App.h"
#include "Views/MainPage.h"
#include <winrt/Windows.Storage.h>

namespace wama = winrt::Windows::ApplicationModel::Activation;
namespace wamc = winrt::Windows::ApplicationModel::Core;
namespace wf = winrt::Windows::Foundation;
namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wux = winrt::Windows::UI::Xaml;

namespace winrt::HotCorner::Uwp::implementation {
	App::App() {
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
		UnhandledException([this](const IInspectable&, const wux::UnhandledExceptionEventArgs& e)
			{
				if (IsDebuggerPresent()) {
					const auto errorMessage = e.Message();
					__debugbreak();
				}
			}
		);
#endif
	}

	void App::OnLaunched(const wama::LaunchActivatedEventArgs&) const {
		const auto view = wuvm::ApplicationView::GetForCurrentView();
		const auto size = wf::Size{ 480, 420 };

		view.SetPreferredMinSize(size);
		view.TryResizeView(size);

		const auto window = wux::Window::Current();
		if (!window.Content()) {
			window.Content(Views::MainPage());
		}

		window.Activate();
	}

	Settings::SettingsManager& App::Settings() {
		static Settings::SettingsManager m_settings{
			Windows::Storage::ApplicationData::Current().LocalFolder().Path().c_str()
		};
		static bool m_settingsLoaded = false;

		if (!m_settingsLoaded) {
			m_settingsLoaded = true;
			m_settings.Load();
		}

		return m_settings;
	}
}
