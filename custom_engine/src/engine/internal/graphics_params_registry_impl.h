// @Note: utility for automatic registration of GVM instructions:
//        - #define a INSTRUCTION_IMPL(T) macro
//        - #include this file
GP_IMPL(graphics::Clear_Flags)
GP_IMPL(graphics::Comparison)
GP_IMPL(graphics::Color_Write)
GP_IMPL(graphics::Operation)
GP_IMPL(graphics::Blend_Mode)
GP_IMPL(graphics::Cull_Mode)
GP_IMPL(graphics::Front_Face)
GP_IMPL(graphics::Texture_Type)
GP_IMPL(graphics::texture_unit)
GP_IMPL(graphics::sampler_unit)
GP_IMPL(graphics::Data_Type)
GP_IMPL(graphics::Filter_Mode)
GP_IMPL(graphics::Wrap_Mode)
GP_IMPL(graphics::Shader_Part)
GP_IMPL(graphics::Mesh_Frequency)
GP_IMPL(graphics::Mesh_Access)
GP_IMPL(graphics::Clip_Origin)
GP_IMPL(graphics::Clip_Depth)
GP_IMPL(graphics::Instruction)
//
#undef GP_IMPL
