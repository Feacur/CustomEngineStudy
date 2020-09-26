#pragma once

// #if __cplusplus < 201103L
// 	// @Note: in case you wanted a version older than C++11 (?)
// 	#define alignof(type) ({\
// 		struct s { char c; type d; };\
// 		offsetof(struct s, d);\
// 	})
// #endif

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

#if defined(_MSC_VER)
	namespace meta {
		template<typename T> struct underlying_type { typedef __underlying_type(T) type; };
		template<typename T> struct is_enum         { static bool const value = __is_enum(T); };
	}
	#define UNDERLYING_TYPE_META(T, U)
	#define IS_ENUM_META(T)
#else
	namespace meta {
		template<typename T> struct underlying_type;
		template<typename T> struct is_enum;
	}
	#define UNDERLYING_TYPE_META(T, U) namespace meta { template<> struct underlying_type<T> { typedef typename U type; }; }
	#define IS_ENUM_META(T)            namespace meta { template<> struct is_enum<T>         { static bool const value = true; }; }

#endif
