// @Note: utility for automatic registration of component types:
//        - #define a COMPONENT_IMPL(T) macro
//        - #include this file
COMPONENT_IMPL(Visual)
COMPONENT_IMPL(Lua_Script)
COMPONENT_IMPL(Physical)
COMPONENT_IMPL(Phys2d)
//
#undef COMPONENT_IMPL
