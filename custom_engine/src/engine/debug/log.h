#pragma once

#include <stdio.h>

// enable ANSI escape codes for CMD: set `HKEY_CURRENT_USER\Console\VirtualTerminalLevel` to `0x00000001`
// use UTF-8: chcp 65001

// https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html

#define ANSI_CLR "\u001b[0m"

#define ANSI_BKG_RED "\u001b[48;5;1m"
#define ANSI_BKG_GRN "\u001b[48;5;2m"
#define ANSI_BKG_YLW "\u001b[48;5;3m"
#define ANSI_BKG_GRY "\u001b[48;5;8m"
#define ANSI_BKG_MGT "\u001b[48;5;13m"
#define ANSI_BKG_CYN "\u001b[48;5;14m"

#define ANSI_TXT_RED "\u001b[38;5;1m"
#define ANSI_TXT_GRN "\u001b[38;5;2m"
#define ANSI_TXT_YLW "\u001b[38;5;3m"
#define ANSI_TXT_GRY "\u001b[38;5;8m"
#define ANSI_TXT_MGT "\u001b[38;5;13m"
#define ANSI_TXT_CYN "\u001b[38;5;14m"

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
	#define CUSTOM_MESSAGE(...)  { fprintf(stderr, __VA_ARGS__); }
	#define CUSTOM_TRACE(...)    { fprintf(stderr, ANSI_TXT_GRY "[trc] " ANSI_CLR); CUSTOM_MESSAGE(__VA_ARGS__); fprintf(stderr, "\n"); }
	#define CUSTOM_WARNING(...)  { fprintf(stderr, ANSI_TXT_YLW "[wrn] " ANSI_CLR); CUSTOM_MESSAGE(__VA_ARGS__); fprintf(stderr, "\n"); }
	#define CUSTOM_ERROR(...)    { fprintf(stderr, ANSI_TXT_RED "[err] " ANSI_CLR); CUSTOM_MESSAGE(__VA_ARGS__); fprintf(stderr, "\n"); }
	#define CUSTOM_CRITICAL(...) { fprintf(stderr, ANSI_BKG_RED "[crt] " ANSI_CLR); CUSTOM_MESSAGE(__VA_ARGS__); fprintf(stderr, "\n"); }
#else
	#define CUSTOM_MESSAGE(...)
	#define CUSTOM_TRACE(...)
	#define CUSTOM_WARNING(...)
	#define CUSTOM_ERROR(...)
	#define CUSTOM_EXCEPTION(...)
#endif

#if !defined(CUSTOM_SHIPPING) && 1
	#define CUSTOM_ASSERT(statement, ...) if(statement) { /**/ } else {\
		CUSTOM_CRITICAL(__VA_ARGS__);\
		CUSTOM_MESSAGE(ANSI_TXT_GRY "  at: " CUSTOM_FILE_AND_LINE ANSI_CLR "\n");\
		CUSTOM_DEBUG_BREAK();\
	}
#else
	#define CUSTOM_ASSERT(statement, ...)
#endif