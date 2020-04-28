// @Note: utility for automatic components registration:
//        - #define a COMPONENT_IMPL(T) macro
//        - #include this file
COMPONENT_IMPL(Visual)
COMPONENT_IMPL(Transform)
COMPONENT_IMPL(Transform2d)
#undef COMPONENT_IMPL
