// @Note: utility for automatic registration of GVM-specific parameters:
//        - #define a GRAPHICS_PARAM_IMPL(T) macro
//        - #include this file
GRAPHICS_PARAM_IMPL(Clear_Flag)
GRAPHICS_PARAM_IMPL(Comparison)
GRAPHICS_PARAM_IMPL(Color_Write)
GRAPHICS_PARAM_IMPL(Operation)
GRAPHICS_PARAM_IMPL(Blend_Mode)
GRAPHICS_PARAM_IMPL(Cull_Mode)
GRAPHICS_PARAM_IMPL(Front_Face)
GRAPHICS_PARAM_IMPL(Texture_Type)
GRAPHICS_PARAM_IMPL(Data_Type)
GRAPHICS_PARAM_IMPL(Filter_Mode)
GRAPHICS_PARAM_IMPL(Wrap_Mode)
GRAPHICS_PARAM_IMPL(Mesh_Frequency)
GRAPHICS_PARAM_IMPL(Mesh_Access)
GRAPHICS_PARAM_IMPL(Clip_Origin)
GRAPHICS_PARAM_IMPL(Clip_Depth)
GRAPHICS_PARAM_IMPL(Instruction)
//
#undef GRAPHICS_PARAM_IMPL
