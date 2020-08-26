#include "engine/core/types.h"
#include "engine/core/math_types.h"

inline static bool IS_BLANK(char value) { return (value == ' ')  || (value == '\t'); }
inline static bool IS_EOL(char value)   { return (value == '\r') || (value == '\n'); }
inline static bool IS_DIGIT(char value) { return (value >= '0')  && (value <= '9'); }

namespace custom {

void parse_void(cstring * source);
void parse_eol(cstring * source, cstring const end);
u32 parse_u32(cstring * source);
s32 parse_s32(cstring * source);
r32 parse_r32(cstring * source);
vec2 parse_vec2(cstring * source);
vec3 parse_vec3(cstring * source);
vec4 parse_vec4(cstring * source);

}
