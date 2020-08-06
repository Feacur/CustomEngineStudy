// @Note: utility for automatic registration of entity components:
//        - #define a COMPONENT_IMPL(T) macro
//        - #include this file
COMPONENT_IMPL(Lua_Script)
COMPONENT_IMPL(Visual)
COMPONENT_IMPL(Transform)
// COMPONENT_IMPL(Hierarchy)
// COMPONENT_IMPL(Camera)
// COMPONENT_IMPL(Transform2d)
// COMPONENT_IMPL(Camera2d)
//
#undef COMPONENT_IMPL
