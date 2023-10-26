#pragma once

// Defines a property with public getter and setter for a WinRT class.
#define PROPERTY(TYPE, NAME)     \
public:                          \
	void NAME(const TYPE &value) \
	{                            \
		m_##NAME## = value;      \
	}                            \
	GET_PROPERTY(TYPE, NAME)

// Defines a property with a public getter for a WinRT class.
#define GET_PROPERTY(TYPE, NAME) \
public:                          \
	TYPE NAME() const            \
	{                            \
		return m_##NAME##;       \
	}                            \
private:                         \
	TYPE m_##NAME

// Defines a static property with public getter and setter for a WinRT class.
#define STATIC_PROPERTY(TYPE, NAME)     \
public:                                 \
	static void NAME(const TYPE &value) \
	{                                   \
		m_##NAME = value;               \
	}                                   \
	STATIC_GET_PROPERTY(TYPE, NAME)

// Defines a static property with a public getter for a WinRT class.
#define STATIC_GET_PROPERTY(TYPE, NAME) \
public:                                 \
	static TYPE NAME()                  \
	{                                   \
		return m_##NAME##;              \
	}                                   \
private:                                \
	static TYPE m_##NAME

#ifdef USE_WINUI3
#define __XAML winrt::Microsoft::UI::Xaml
#else
#define __XAML winrt::Windows::UI::Xaml
#endif

// Helper macro for property metadata.
// Do not use in authored code.
#define __DEPENDENCY_PROPERTY_METADATA(VALUE, CHANGE_HANDLER) \
__XAML::PropertyMetadata(VALUE, CHANGE_HANDLER)

// Base macro for DependencyProperty registration.
// Do not use in authored code.
#define __DEPENDENCY_PROPERTY_BASE(NAME, TYPE, METADATA)                \
public:                                                                 \
	static __XAML::DependencyProperty NAME##Property() noexcept         \
	{                                                                   \
		return m_##NAME##Property;                                      \
	}                                                                   \
	TYPE NAME() const                                                   \
	{                                                                   \
		return winrt::unbox_value<TYPE>(GetValue(m_##NAME##Property));  \
	}                                                                   \
	void NAME(const TYPE &value) const                                  \
	{                                                                   \
		SetValue(m_##NAME##Property, winrt::box_value(value));          \
	}                                                                   \
private:                                                                \
	inline static __XAML::DependencyProperty const m_##NAME##Property = \
		__XAML::DependencyProperty::Register                            \
		(                                                               \
			L#NAME,                                                     \
			winrt::xaml_typename<TYPE>(),                               \
			winrt::xaml_typename<class_type>(),                         \
			METADATA                                                    \
		)

// Base macro for attached DependencyProperty registration.
// Do not use in authored code.
#define __ATTACHED_PROPERTY_BASE(NAME, TYPE, TARGETTYPE, METADATA)            \
public:                                                                       \
	static __XAML::DependencyProperty NAME##Property() noexcept               \
	{                                                                         \
		return m_##NAME##Property;                                            \
	}                                                                         \
	static TYPE Get##NAME(const TARGETTYPE &target)                           \
	{                                                                         \
		return winrt::unbox_value<TYPE>(target.GetValue(m_##NAME##Property)); \
	}                                                                         \
	static void Set##NAME(const TARGETTYPE &target, const TYPE &value)        \
	{                                                                         \
		target.SetValue(m_##NAME##Property, winrt::box_value(value));         \
	}                                                                         \
private:                                                                      \
	inline static __XAML::DependencyProperty const m_##NAME##Property =       \
		__XAML::DependencyProperty::RegisterAttached                          \
		(                                                                     \
			L#NAME,                                                           \
			winrt::xaml_typename<TYPE>(),                                     \
			winrt::xaml_typename<class_type>(),                               \
			METADATA                                                          \
		)

// Defines a dependency property for a WinRT class.
#define DEPENDENCY_PROPERTY(NAME, TYPE)         \
__DEPENDENCY_PROPERTY_BASE(NAME, TYPE, nullptr)

// Defines a dependency property with property metadata for a WinRT class.
#define DEPENDENCY_PROPERTY_META(NAME, TYPE, VALUE, CHANGE_HANDLER) \
__DEPENDENCY_PROPERTY_BASE                                          \
(                                                                   \
	NAME,                                                           \
	TYPE,                                                           \
	__DEPENDENCY_PROPERTY_METADATA(VALUE, CHANGE_HANDLER)           \
)

// Defines an attached property for a WinRT class.
#define ATTACHED_PROPERTY(NAME, TYPE, TARGETTYPE)         \
__ATTACHED_PROPERTY_BASE(NAME, TYPE, TARGETTYPE, nullptr)

// Defines an attached property with property metadata for a WinRT class.
#define ATTACHED_PROPERTY_META(NAME, TYPE, TARGETTYPE, VALUE, CHANGE_HANDLER) \
__ATTACHED_PROPERTY_BASE                                                      \
(                                                                             \
	NAME,                                                                     \
	TYPE,                                                                     \
	TARGETTYPE,                                                               \
	__DEPENDENCY_PROPERTY_METADATA(VALUE, CHANGE_HANDLER)                     \
)
