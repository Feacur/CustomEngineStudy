#include "engine/core/types.h"
#include "engine/core/math_types.h"

namespace custom {

inline static bool IS_BLANK(char value) { return (value == ' ')  || (value == '\t'); }
inline static bool IS_EOL(char value)   { return (value == '\r') || (value == '\n'); }
inline static bool IS_DIGIT(char value) { return (value >= '0')  && (value <= '9'); }
inline static cstring skip_blank(cstring line) { while (IS_BLANK(*line)) { ++line; } return line; }

u32 parse_u32(u32 value, cstring line, cstring * next_out);
s32 parse_s32(cstring line, cstring * next_out);
r32 parse_r32(cstring line, cstring * next_out);

}
