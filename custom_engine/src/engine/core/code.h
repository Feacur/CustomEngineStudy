#pragma once
#include <string.h>

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

#define CALL_SAFELY(func_ptr, ...) if (func_ptr) (*func_ptr)(__VA_ARGS__)

#if !defined(__cplusplus)
	#define constexpr
#endif

#define CUSTOM_CONDITIONAL(statement, command, ...)                              \
if (!(statement)) { /**/ } else {                                                \
    CUSTOM_CRITICAL(__VA_ARGS__);                                                \
    CUSTOM_MESSAGE("  " ANSI_TXT_GRY "at: " CUSTOM_FILE_AND_LINE ANSI_CLR "\n"); \
    command;                                                                     \
}                                                                                \

#define CUSTOM_GET_NEXT_MULTIPLE_2(value, multiple) ((value + (multiple - 1)) & ~(multiple - 1))
#define CUSTOM_ALIGN(value, alignment) CUSTOM_GET_NEXT_MULTIPLE_2(value, (decltype(value))alignment)

