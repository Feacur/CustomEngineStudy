#pragma once
#include <float.h>  // floating point conveniences
#include <stdio.h>  // file operations, printing functions
#include <string.h> // raw memory operations
#include <math.h>   // non-trivial math functions

// debug intrinsics, SIMD
#if defined(_MSC_VER)
	#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
	#include <x86intrin.h>
#else
	#pragma message("no intrinsics header included")
#endif

// macros
#define CUSTOM_STRINGIFY_A_VALUE(v) #v
#define CUSTOM_STRINGIFY_A_MACRO(m) CUSTOM_STRINGIFY_A_VALUE(m)
#define CUSTOM_TOKENIZE_A_VALUE(v1, v2) v1 ## v2
#define CUSTOM_TOKENIZE_A_MACRO(m1, m2) CUSTOM_TOKENIZE_A_VALUE(m1, m2)

#define CUSTOM_FILE_AND_LINE __FILE__ ":" CUSTOM_STRINGIFY_A_MACRO(__LINE__)
#define C_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#if !defined(__cplusplus)
	#define constexpr
#endif

#if !defined(NOMINMAX)
	// @Note: <Windows.h> leaks junk even with WIN32_LEAN_AND_MEAN
	#undef min
	#undef max
#endif

// OS detection
#if defined(_WIN64) || defined(_WIN32)
	#define CUSTOM_OS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#define CUSTOM_OS_IOS_SIMULATOR
	#elif TARGET_OS_IPHONE == 1
		#define CUSTOM_OS_IOS
	#elif TARGET_OS_MAC == 1
		#define CUSTOM_OS_MACOS
	#else
		#error "unknown Apple platform!"
	#endif
#elif defined(__ANDROID__)
	#define CUSTOM_OS_ANDROID
#elif defined(__linux__)
	#define CUSTOM_OS_LINUX
#else
	#error "unknown platform"
#endif

// shared library interface
#if defined(_MSC_VER)
	#define CUSTOM_DLL_EXPORT __declspec(dllexport)
	#define CUSTOM_DLL_IMPORT __declspec(dllimport)
#else
	#define CUSTOM_DLL_EXPORT
	#define CUSTOM_DLL_IMPORT
#endif

#if defined(CUSTOM_SYMBOLS_SHARE)
	#if defined(CUSTOM_SHARED_LIBRARY)
		#define CUSTOM_EXTERN
		#define CUSTOM_DLL CUSTOM_DLL_EXPORT
	#else
		#define CUSTOM_EXTERN extern
		#define CUSTOM_DLL CUSTOM_DLL_IMPORT
	#endif
#else
	#define CUSTOM_EXTERN
	#define CUSTOM_DLL
#endif
