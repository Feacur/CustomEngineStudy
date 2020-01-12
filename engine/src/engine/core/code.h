#pragma once

#include <intrin.h> // debug intrinsics, SIMD
#include <stdint.h> // integer data types
#include <float.h>  // floating point data types
#include <stdio.h>  // file operations, printing functions
#include <string.h> // raw memory operations
#include <math.h>   // non-trivial math functions

// basic types
typedef int8_t   s8;  // signed char
typedef int16_t  s16; // short
typedef int32_t  s32; // int
typedef int64_t  s64; // long long

typedef uint8_t  u8;  // unsigned char
typedef uint16_t u16; // unsigned short
typedef uint32_t u32; // unsigned int
typedef uint64_t u64; // unsigned long long

typedef long          s48; // witty (32 + 64) / 2
typedef unsigned long u48; // witty (32 + 64) / 2

typedef float  r32;
typedef double r64;

typedef char const * cstring;

// macros
#define CUSTOM_STRINGIFY_A_VALUE(v) #v
#define CUSTOM_STRINGIFY_A_MACRO(m) CUSTOM_STRINGIFY_A_VALUE(m)
#define CUSTOM_TOKENIZE_A_VALUE(v1, v2) v1 ## v2
#define CUSTOM_TOKENIZE_A_MACRO(m1, m2) CUSTOM_TOKENIZE_A_VALUE(m1, m2)

#define CUSTOM_FILE_AND_LINE __FILE__ ":" CUSTOM_STRINGIFY_A_MACRO(__LINE__)
#define C_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#undef min
#undef max

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

#if defined(CUSTOM_SHARED)
	#if defined(CUSTOM_IS_DLL)
		#define CUSTOM_TEMPLATE
		#define CUSTOM_DLL CUSTOM_DLL_EXPORT
	#else
		#define CUSTOM_TEMPLATE extern
		#define CUSTOM_DLL CUSTOM_DLL_IMPORT
	#endif
#else
	#define CUSTOM_TEMPLATE
	#define CUSTOM_DLL
#endif

// c++ meta and operators for enum structs
#if defined(__cplusplus)
namespace meta {
	template<size_t S> struct uint_for_size;
	template<> struct uint_for_size<1> { typedef u8  type; };
	template<> struct uint_for_size<2> { typedef u16 type; };
	template<> struct uint_for_size<4> { typedef u32 type; };
	template<> struct uint_for_size<8> { typedef u64 type; };

	template<typename T>
	struct uint_for_type {
		typedef typename uint_for_size<sizeof(T)>::type type;
	};
}

#if defined(_MSC_VER)
	namespace meta {
		template<typename T>
		struct underlying_type {
			typedef typename __underlying_type(T) type;
		};

		template<typename T>
		struct is_enum {
			static bool const value = __is_enum(T);
		};
	}
	#define UNDERLYING_TYPE_META(T, U)
	#define IS_ENUM_META(T)
#else
	namespace meta {
		template<typename T> struct underlying_type;
		template<typename T> struct is_enum;
	}
	#define UNDERLYING_TYPE_META(T, U)\
	namespace meta { template<> struct underlying_type<T> { typedef typename U type; }; }

	#define IS_ENUM_META(T)\
	namespace meta { template<> struct is_enum<T> { static bool const value = true; }; }
#endif

#define ENUM_FLAG_OPERATORS_IMPL(T)\
constexpr inline T operator&(T a, T b) {\
	using U = meta::uint_for_type<T>::type;\
	return static_cast<T>(static_cast<U>(a) & static_cast<U>(b));\
}\
constexpr inline T operator|(T a, T b) {\
	using U = meta::uint_for_type<T>::type;\
	return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));\
}\
constexpr inline T operator^(T a, T b) {\
	using U = meta::uint_for_type<T>::type;\
	return static_cast<T>(static_cast<U>(a) ^ static_cast<U>(b));\
}\
constexpr inline T operator~(T v) {\
	using U = meta::uint_for_type<T>::type;\
	return static_cast<T>(~static_cast<U>(v));\
}\
constexpr inline bool bits_are_set(T container, T bits) { return (container & bits) == bits; }\
constexpr inline T bits_to_zero(T container, T bits) { return container & ~bits; }
#endif // defined(__cplusplus)
