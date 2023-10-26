#pragma once

#ifdef USE_WINUI3
#define __MIDL_XAML Microsoft.UI.Xaml
#else
#define __MIDL_XAML Windows.UI.Xaml
#endif

// Defines a dependency property for a runtime class.
#define DEPENDENCY_PROPERTY(NAME, TYPE)                              \
[noexcept]                                                           \
static __MIDL_XAML.DependencyProperty NAME ## Property { get; }; \
TYPE NAME

// Defines an attached property for a runtime class.
#define ATTACHED_PROPERTY(NAME, TYPE, TARGETTYPE)                    \
[noexcept]                                                           \
static __MIDL_XAML.DependencyProperty NAME ## Property { get; }; \
static TYPE Get ## NAME ## (TARGETTYPE target);                      \
static void Set ## NAME ## (TARGETTYPE target, TYPE value)
