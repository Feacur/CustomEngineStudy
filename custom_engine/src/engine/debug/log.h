#pragma once

#include <stdio.h>

// @Note: undef if want shipping mode logs and assertions
#if !defined(CUSTOM_SHIPPING)
	#define LOG_MESSAGE_ENABLED
	#define LOG_ASSERT_ENABLED
#endif

// enable ANSI escape codes for CMD: set `HKEY_CURRENT_USER\Console\VirtualTerminalLevel` to `0x00000001`
// use UTF-8: chcp 65001

// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
// https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html

#if 1
	#define ANSI_ESC "\x1b"
	#define ANSI_CLR ANSI_ESC "[0m"

	#define ANSI_BKG_RED ANSI_ESC "[48;5;1m"
	#define ANSI_BKG_GRN ANSI_ESC "[48;5;2m"
	#define ANSI_BKG_YLW ANSI_ESC "[48;5;3m"
	#define ANSI_BKG_GRY ANSI_ESC "[48;5;8m"
	#define ANSI_BKG_MGT ANSI_ESC "[48;5;13m"
	#define ANSI_BKG_CYN ANSI_ESC "[48;5;14m"
	#define ANSI_BKG_ORG ANSI_ESC "[48;5;202m"

	#define ANSI_TXT_RED ANSI_ESC "[38;5;1m"
	#define ANSI_TXT_GRN ANSI_ESC "[38;5;2m"
	#define ANSI_TXT_YLW ANSI_ESC "[38;5;3m"
	#define ANSI_TXT_GRY ANSI_ESC "[38;5;8m"
	#define ANSI_TXT_MGT ANSI_ESC "[38;5;13m"
	#define ANSI_TXT_CYN ANSI_ESC "[38;5;14m"
	#define ANSI_TXT_ORG ANSI_ESC "[38;5;202m"
#else
	#define ANSI_ESC
	#define ANSI_CLR

	#define ANSI_BKG_RED
	#define ANSI_BKG_GRN
	#define ANSI_BKG_YLW
	#define ANSI_BKG_GRY
	#define ANSI_BKG_MGT
	#define ANSI_BKG_CYN

	#define ANSI_TXT_RED
	#define ANSI_TXT_GRN
	#define ANSI_TXT_YLW
	#define ANSI_TXT_GRY
	#define ANSI_TXT_MGT
	#define ANSI_TXT_CYN
#endif

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

#if defined(LOG_MESSAGE_ENABLED)
	#define CUSTOM_MESSAGE(...)  do { printf(__VA_ARGS__); } while (0)
	#define CUSTOM_TRACE(...)    do { CUSTOM_MESSAGE(ANSI_TXT_GRY "[trc]" ANSI_CLR " "); CUSTOM_MESSAGE(__VA_ARGS__); CUSTOM_MESSAGE("\n"); } while (0)
	#define CUSTOM_WARNING(...)  do { CUSTOM_MESSAGE(ANSI_TXT_YLW "[wrn]" ANSI_CLR " "); CUSTOM_MESSAGE(__VA_ARGS__); CUSTOM_MESSAGE("\n"); } while (0)
	#define CUSTOM_ERROR(...)    do { CUSTOM_MESSAGE(ANSI_TXT_RED "[err]" ANSI_CLR " "); CUSTOM_MESSAGE(__VA_ARGS__); CUSTOM_MESSAGE("\n"); } while (0)
	#define CUSTOM_CRITICAL(...) do { CUSTOM_MESSAGE(ANSI_BKG_RED "[crt]" ANSI_CLR " "); CUSTOM_MESSAGE(__VA_ARGS__); CUSTOM_MESSAGE("\n"); } while (0)
#else
	#define CUSTOM_MESSAGE(...)  (void)0
	#define CUSTOM_TRACE(...)    (void)0
	#define CUSTOM_WARNING(...)  (void)0
	#define CUSTOM_ERROR(...)    (void)0
	#define CUSTOM_CRITICAL(...) (void)0
#endif

#if defined(LOG_ASSERT_ENABLED)
	#define CUSTOM_ASSERT(statement, ...) CUSTOM_CONDITIONAL(!(statement), CUSTOM_DEBUG_BREAK(), __VA_ARGS__)
#else
	#define CUSTOM_ASSERT(statement, ...) (void)0
#endif