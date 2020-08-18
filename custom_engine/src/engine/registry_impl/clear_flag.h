// @Note: utility for automatic registration of clear flags:
//        - #define a CLEAR_FLAG_IMPL(T) macro
//        - #include this file
CLEAR_FLAG_IMPL(Color,   0)
CLEAR_FLAG_IMPL(Depth,   1)
CLEAR_FLAG_IMPL(Stencil, 2)
//
#undef CLEAR_FLAG_IMPL
