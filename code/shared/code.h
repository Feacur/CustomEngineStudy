#define SHARED_CODE

#include <intrin.h> // __debugbreak(), SIMD
#include <stdint.h> // integer types data
#include <float.h>  // floating point types data
#include <stdio.h>  // printf
#include <string.h> // memset, memcpy
#include <math.h>   // sqrtf, sinf, cosf
// #include <memory>   // std::unique_ptr, std::shared_ptr

// basic types
typedef int8_t   int8;  // signed char
typedef int16_t  int16; // short
typedef int32_t  int32; // int
typedef int64_t  int64; // long long

typedef uint8_t  uint8;  // unsigned char
typedef uint16_t uint16; // unsigned short
typedef uint32_t uint32; // unsigned int
typedef uint64_t uint64; // unsigned long long

typedef long          int48;  // witty (32 + 64) / 2
typedef unsigned long uint48; // witty (32 + 64) / 2

typedef char const * cstring;

// macros
#define STRINGIFY_A_VALUE(VALUE) #VALUE
#define STRINGIFY_A_MACRO(MACRO) STRINGIFY_A_VALUE(MACRO)
#define FILE_AND_LINE __FILE__ ":" STRINGIFY_A_MACRO(__LINE__)
#define C_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#undef min
#undef max

// language-specific
#if defined(__cplusplus)
	#define API_C extern "C"
	#define API_C_BLOCK_START extern "C" {
	#define API_C_BLOCK_END }
#else
	#define API_C
	#define API_C_BLOCK_START
	#define API_C_BLOCK_END
	#define constexpr
	#define thread_local
#endif

// platform-specific
#if defined(_WIN32)
	#if defined(_WIN64)
		#define PLATFORM_WINDOWS
	#else
		#define PLATFORM_WINDOWS
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
	#if TARGET_IPHONE_SIMULATOR == 1
		#define PLATFORM_IOS_SIMULATOR
	#elif TARGET_OS_IPHONE == 1
		#define PLATFORM_IOS
	#elif TARGET_OS_MAC == 1
		#define PLATFORM_MACOS
	#else
		#error "Unknown Apple platform!"
	#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
	#define PLATFORM_ANDROID
#elif defined(__linux__)
	#define PLATFORM_LINUX
#else
	#error "Unknown compiler/platform!"
#endif

// compiler-specific
#if defined(_MSC_VER)
	#define CODE_BREAK() __debugbreak()
	#if defined(API_SHARED)
		#if defined(BUILD_DLL)
			#define API_DLL __declspec(dllexport)
			#define API_TEMPLATE
		#else
			#define API_DLL __declspec(dllimport)
			#define API_TEMPLATE extern
		#endif
	#else
		#define API_DLL
		#define API_TEMPLATE
	#endif
#else
	#error supported platforms: Windows
#endif

// logging
#if defined(SHIPPING)
	#define LOG_TRACE(MESSAGE)
	#define LOG_WARNING(MESSAGE)
	#define LOG_ERROR(MESSAGE)
	#define ASSERT_TRUE(STATEMENT, MESSAGE)
#else
	inline void logger_message(cstring type, cstring location, cstring message) {
		printf("%s: %s\n\t---> %s\n", type, message, location);
	}
	#define LOG_TRACE(MESSAGE)   logger_message("TRACE",   FILE_AND_LINE, MESSAGE)
	#define LOG_WARNING(MESSAGE) logger_message("WARNING", FILE_AND_LINE, MESSAGE)
	#define LOG_ERROR(MESSAGE)   logger_message("ERROR",   FILE_AND_LINE, MESSAGE)
	#define ASSERT_TRUE(STATEMENT, MESSAGE) { if(STATEMENT) { /**/ } else { LOG_ERROR(MESSAGE); CODE_BREAK(); } }
#endif

// traits
#if defined(__cplusplus) // meta
	#if defined(_MSC_VER)
		namespace meta {
			template<typename T>
			struct underlying_type {
				typedef __underlying_type(T) type;
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
			template<typename T> struct underlying_type { };
			template<typename T> struct is_enum { };
		}
		#define UNDERLYING_TYPE_META(T, U)\
		namespace meta { template<> struct underlying_type<T> { typedef U type; }; }

		#define IS_ENUM_META(T)\
		namespace meta { template<> struct is_enum<T> { static bool const value = true; }; }
	#endif
#endif // defined(__cplusplus) // meta
