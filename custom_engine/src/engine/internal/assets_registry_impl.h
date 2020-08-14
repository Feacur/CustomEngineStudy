// @Note: utility for automatic registration of asset types:
//        - #define a ASSET_IMPL(T) macro
//        - #include this file
ASSET_IMPL(ShaderAsset);
ASSET_IMPL(TextureAsset);
ASSET_IMPL(MeshAsset);
//
#undef ASSET_IMPL
