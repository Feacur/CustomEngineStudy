#pragma once

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

// @Note: forces using a static format string
#define CUSTOM_FORMAT(kind, format) "[" #kind "]: " format "\n\tat: " CUSTOM_FILE_AND_LINE "\n"

#if !defined(CUSTOM_SHIPPING)
	#define CUSTOM_TRACE(format, ...) fprintf(stdout, CUSTOM_FORMAT(trace, format), __VA_ARGS__);
	#define CUSTOM_INFO(format, ...)  fprintf(stdout, CUSTOM_FORMAT(info, format), __VA_ARGS__);
	#define CUSTOM_WARN(format, ...)  fprintf(stdout, CUSTOM_FORMAT(warn, format), __VA_ARGS__);
	#define CUSTOM_ERROR(format, ...) fprintf(stderr, CUSTOM_FORMAT(error, format), __VA_ARGS__);
	#define CUSTOM_ASSERT(statement, format, ...)\
		if(statement) { /**/ } else { fprintf(stderr, CUSTOM_FORMAT(critical, format), __VA_ARGS__); CUSTOM_DEBUG_BREAK(); }
#else
	#define CUSTOM_TRACE(format, ...)
	#define CUSTOM_INFO(format, ...)
	#define CUSTOM_WARN(format, ...)
	#define CUSTOM_ERROR(format, ...)
	#define CUSTOM_ASSERT(statement, format, ...)
#endif
