#pragma once
#include "engine/core/types.h"
#include "engine/core/math_types.h"

inline static bool IS_BLANK(char value)  { return (value == ' ')  || (value == '\t'); }
inline static bool IS_EOL(char value)    { return (value == '\r') || (value == '\n'); }
inline static bool IS_VOID(char value)   { return IS_BLANK(value) || IS_EOL(value); }
inline static bool IS_LOWER(char value)  { return (value >= 'a')  && (value <= 'z'); }
inline static bool IS_UPPER(char value)  { return (value >= 'A')  && (value <= 'Z'); }
inline static bool IS_LETTER(char value) { return IS_LOWER(value) || IS_UPPER(value); }
inline static bool IS_DIGIT(char value)  { return (value >= '0')  && (value <= '9'); }
inline static bool IS_VALID_IDENTIFIER_START(char value) { return IS_LETTER(value) || (value == '_'); }
inline static bool IS_IDENTIFIER_SYMBOL(char value) { return IS_VALID_IDENTIFIER_START(value) || IS_DIGIT(value); }
inline static bool IS_QUOTE(char value) { return (value == '"'); }
inline static bool IS_SINGLE_LINE_STRING_END(char value) { return IS_QUOTE(value) || IS_EOL(value); }

namespace custom {

void skip_to_blank(cstring * source);
void skip_to_eol(cstring * source);
void skip_to_void(cstring * source);
void skip_to_digit(cstring * source);
void skip_to_single_line_string_end(cstring * source);

void parse_blank(cstring * source);
void parse_eol(cstring * source);
void parse_void(cstring * source);
void parse_identifier(cstring * source);

u32 parse_u32(cstring * source);
s32 parse_s32(cstring * source);
bln parse_bln(cstring * source);
r32 parse_r32(cstring * source);

vec2 parse_vec2(cstring * source);
vec3 parse_vec3(cstring * source);
vec4 parse_vec4(cstring * source);

}
