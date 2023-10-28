#pragma once

#ifdef USE_WINUI3
#define XAML Microsoft.UI.Xaml
#else
#define XAML Windows.UI.Xaml
#endif

#define MUX Microsoft.UI.Xaml

// Defines a dependency property for a runtime class.
#define DEPENDENCY_PROPERTY(NAME, TYPE)                              \
[noexcept]                                                           \
static XAML.DependencyProperty NAME ## Property { get; }; \
TYPE NAME

// Defines an attached property for a runtime class.
#define ATTACHED_PROPERTY(NAME, TYPE, TARGETTYPE)                    \
[noexcept]                                                           \
static XAML.DependencyProperty NAME ## Property { get; }; \
static TYPE Get ## NAME ## (TARGETTYPE target);                      \
static void Set ## NAME ## (TARGETTYPE target, TYPE value)
