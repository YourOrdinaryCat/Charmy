#pragma once
#include <concepts>
#include <winrt/Windows.Foundation.h>

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Xaml.h>
namespace xaml = winrt::Microsoft::UI::Xaml;
#else
#include <winrt/Windows.UI.Xaml.h>
namespace xaml = winrt::Windows::UI::Xaml;
#endif

template<typename T>
concept is_winrt_type = std::is_base_of_v<
	winrt::Windows::Foundation::IInspectable,
	std::decay_t<T>
>;

template<typename T>
concept is_hstring = std::is_same_v<winrt::hstring, std::decay_t<T>>;

template<typename T>
struct decay_non_winrt {
	using type = std::decay_t<T>;
};

template<is_hstring THS>
struct decay_non_winrt<THS> {
	using type = THS;
};

template<is_winrt_type TNWR>
struct decay_non_winrt<TNWR> {
	using type = TNWR;
};

template<typename T>
using decay_non_winrt_t = decay_non_winrt<T>::type;

// Helper macro for property metadata.
// Do not use in authored code.
#define __DEPENDENCY_PROPERTY_METADATA(VALUE, CHANGE_HANDLER) \
xaml::PropertyMetadata(VALUE, CHANGE_HANDLER)

// Defines the public API for a dependency property.
#define DEPENDENCY_PROPERTY_API(NAME, TYPE) \
public: \
	static xaml::DependencyProperty NAME##Property() noexcept { \
		return m_##NAME##Property; \
	} \
	TYPE NAME() const { \
		return winrt::unbox_value<TYPE>(GetValue(m_##NAME##Property)); \
	} \
	void NAME(decay_non_winrt_t<const TYPE&> value) const { \
		SetValue(m_##NAME##Property, winrt::box_value(value)); \
	} \
private: \
	inline static xaml::DependencyProperty m_##NAME##Property = nullptr

// Defines the public API for an attached property.
#define ATTACHED_PROPERTY_API(NAME, TYPE, TARGET) \
public: \
	static xaml::DependencyProperty NAME##Property() noexcept { \
		return m_##NAME##Property; \
	} \
	static TYPE Get##NAME(const TARGET& target) { \
		return winrt::unbox_value<TYPE>(target.GetValue(m_##NAME##Property)); \
	} \
	static void Set##NAME(const TARGET& target, decay_non_winrt_t<const TYPE&> value) { \
		target.SetValue(m_##NAME##Property, winrt::box_value(value)); \
	} \
private: \
	inline static xaml::DependencyProperty m_##NAME##Property = nullptr

// Base macro for DependencyProperty registration.
// Do not use in authored code.
#define __REGISTER_DEPENDENCY_PROPERTY(METHOD, NAME, TYPE, METADATA) \
xaml::DependencyProperty::METHOD \
( \
	L#NAME, \
	winrt::xaml_typename<TYPE>(), \
	winrt::xaml_typename<class_type>(), \
	METADATA \
)

// Defines a dependency property for a WinRT class.
#define REGISTER_DEPENDENCY_PROPERTY(NAME, TYPE) \
m_##NAME##Property = \
	__REGISTER_DEPENDENCY_PROPERTY(Register, NAME, TYPE, nullptr)

// Defines a dependency property with property metadata for a WinRT class.
#define REGISTER_DEPENDENCY_PROPERTY_META(NAME, TYPE, VALUE, CHANGE_HANDLER) \
m_##NAME##Property = __REGISTER_DEPENDENCY_PROPERTY \
( \
	Register, \
	NAME, \
	TYPE, \
	__DEPENDENCY_PROPERTY_METADATA(VALUE, CHANGE_HANDLER) \
)

// Defines an attached property for a WinRT class.
#define REGISTER_ATTACHED_PROPERTY(NAME, TYPE) \
m_##NAME##Property = \
	__REGISTER_DEPENDENCY_PROPERTY(RegisterAttached, NAME, TYPE, nullptr)

// Defines an attached property with property metadata for a WinRT class.
#define REGISTER_ATTACHED_PROPERTY_META(NAME, TYPE, VALUE, CHANGE_HANDLER) \
m_##NAME##Property = __REGISTER_DEPENDENCY_PROPERTY \
( \
	RegisterAttached, \
	NAME, \
	TYPE, \
	__DEPENDENCY_PROPERTY_METADATA(VALUE, CHANGE_HANDLER) \
)

namespace winrt {
	namespace Microsoft::UI::Xaml {
		namespace Controls {}
	}

	namespace Windows::UI::Xaml {
		namespace Controls {}
	}
}

namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = mux::Controls;
namespace wux = xaml;
namespace wuxc = wux::Controls;
