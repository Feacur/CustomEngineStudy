#pragma once

#include <stdio.h>

// enable ANSI escape codes for CMD: set `HKEY_CURRENT_USER\Console\VirtualTerminalLevel` to `0x00000001`
// use UTF-8: chcp 65001

#if defined(_MSC_VER)
	#define CUSTOM_DEBUG_BREAK() __debugbreak()
#else
	#define CUSTOM_DEBUG_BREAK()
#endif

#if defined(_MSC_VER)
	#define CUSTOM_FUNCTION_NAME __FUNCTION__
#else
	#define CUSTOM_FUNCTION_NAME
#endif

#if (__cplusplus >= 201100)
	#define CUSTOM_STATIC_ASSERT(statement) static_assert(statement, "")
#else // @Note: typedef a char array of length 1 or -1, which would be a compile-time error
	#define CUSTOM_STATIC_ASSERT(statement) typedef char static_assertion_##__LINE__[(statement) ? 1 : -1]
#endif

#if !defined(CUSTOM_SHIPPING)
	#define CUSTOM_MESSAGE(...) { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
	#define CUSTOM_REPORT_LINE() CUSTOM_MESSAGE("  at: " CUSTOM_FILE_AND_LINE)
#else
	#define CUSTOM_MESSAGE(...)
	#define CUSTOM_REPORT_LINE(...)
#endif

#if !defined(CUSTOM_SHIPPING) && 1
	#define CUSTOM_ASSERT(statement, ...) if(statement) { /**/ } else {\
		CUSTOM_MESSAGE(__VA_ARGS__);\
		CUSTOM_REPORT_LINE();\
		CUSTOM_DEBUG_BREAK();\
	}
#else
	#define CUSTOM_ASSERT(statement, ...)
#endif