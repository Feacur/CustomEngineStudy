// @Note: utility for automatic registration of asset types:
//        - #define a ASSET_IMPL(T) macro
//        - #include this file
ASSET_IMPL(Shader_Asset)
ASSET_IMPL(Texture_Asset)
ASSET_IMPL(Mesh_Asset)
//
#undef ASSET_IMPL
