// @Note: utility for automatic registration of uniforms:
//        - #define a UNIFORM_IMPL(T) macro
//        - #include this file
UNIFORM_IMPL(Resolution)
UNIFORM_IMPL(View_Projection)
UNIFORM_IMPL(Transform)
UNIFORM_IMPL(Texture)
UNIFORM_IMPL(Color)
UNIFORM_IMPL(Z)
//
#undef UNIFORM_IMPL
