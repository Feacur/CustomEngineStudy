// @Note: utility for automatic registration of GVM-related data types:
//        - #define a DATA_TYPE_IMPL(T) macro
//        - #include this file
DATA_TYPE_IMPL(unit_id)
DATA_TYPE_IMPL(s8)
DATA_TYPE_IMPL(s16)
DATA_TYPE_IMPL(s32)
DATA_TYPE_IMPL(u8)
DATA_TYPE_IMPL(u16)
DATA_TYPE_IMPL(u32)
DATA_TYPE_IMPL(r32)
DATA_TYPE_IMPL(r64)
DATA_TYPE_IMPL(vec2)
DATA_TYPE_IMPL(vec3)
DATA_TYPE_IMPL(vec4)
DATA_TYPE_IMPL(ivec2)
DATA_TYPE_IMPL(ivec3)
DATA_TYPE_IMPL(ivec4)
DATA_TYPE_IMPL(uvec2)
DATA_TYPE_IMPL(uvec3)
DATA_TYPE_IMPL(uvec4)
DATA_TYPE_IMPL(mat2)
DATA_TYPE_IMPL(mat3)
DATA_TYPE_IMPL(mat4)
//
#undef DATA_TYPE_IMPL
