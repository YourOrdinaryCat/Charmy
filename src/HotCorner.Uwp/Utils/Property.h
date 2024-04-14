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
