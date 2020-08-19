// @Note: utility for automatic registration of asset types:
//        - #define a ASSET_IMPL(T) macro
//        - #include this file
ASSET_IMPL(Lua_Asset)
ASSET_IMPL(Prefab)
//
#undef ASSET_IMPL
