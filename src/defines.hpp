#pragma once

// Types (yeah i like rust)
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned long usize;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
typedef signed long isize;

typedef float f32;
typedef double f64;

#define TRUE 1
#define FALSE 0
#define NULL 0

typedef wchar_t wchar;

#ifdef UNICODE
typedef wchar TCHAR;
#define TCOUT wcout
#define TEXT(x) L##x

#else
typedef char TCHAR;
#define TCOUT cout
#define TEXT(x) x
#endif

#define TOSTRINGSTREAM std::basic_ostringstream<TCHAR>
#define TSTRING std::basic_string<TCHAR>

#define BIT(x) 1 << x

// Platform specific
#ifdef PS_PLATFORM_WINDOWS

#elif defined(PS_PLATFORM_LINUX)

#elif defined(PS_PLATFORM_MACOSX)

#elif defined(PS_PLATFORM_IOS)

#elif defined(PS_PLATFORM_ANDROID)

#endif

// Configurations
#ifdef PS_DEBUG
#ifdef PS_TOOLSET_CLANG
#if __has_builtin(__builtin_debugtrap)
#define DEBUG_BREAK __builtin_debugtrap();
#else
#include <signal.h>
#ifdef SIGTRAP
#define DEBUG_BREAK raise(SIGTRAP);
#else
#define DEBUG_BREAK raise(SIGABRT);
#endif
#endif

#elif defined(PS_TOOLSET_MSVC)
#define DEBUG_BREAK __debugbreak();

#elif defined(PS_TOOLSET_GNU)
#define DEBUG_BREAK __builtin_trap();

#else
//#error "Unknown compiler"
#endif
#elif defined(PS_RELEASE)
#define DEBUG_BREAK

#elif defined(PS_DEPLOY)
#define DEBUG_BREAK

#endif