#pragma once

#if defined(_MSC_VER)
	#define CUSTOM_DEBUG_BREAK() __debugbreak()
#else
	#define CUSTOM_DEBUG_BREAK()
#endif

#if defined(_MSC_VER)
	#define FUNCTION_NAME __FUNCTION__
#else
	#define FUNCTION_NAME
#endif

#define CUSTOM_FORMAT(kind, format) "[" #kind "]: " format "\n\tat: " CUSTOM_FILE_AND_LINE "\n"

#if !defined(CUSTOM_SHIPPING)
	#define CUSTOM_TRACE(format, ...) fprintf(stdout, CUSTOM_FORMAT(trace, format), __VA_ARGS__);
	#define CUSTOM_INFO(format, ...) fprintf(stdout, CUSTOM_FORMAT(info, format), __VA_ARGS__);
	#define CUSTOM_WARN(format, ...) fprintf(stdout, CUSTOM_FORMAT(warn, format), __VA_ARGS__);
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
