#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/bytecode.h"
#include "engine/api/internal/renderer.h"
#include "engine/api/graphics_params.h"
#include "engine/api/internal/reference.h"

// http://www.reedbeta.com/blog/depth-precision-visualized/
// https://developer.nvidia.com/content/depth-precision-visualized
// https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
// http://dev.theomader.com/depth-precision/
// https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html
#define REVERSED_Z

typedef custom::graphics::unit_id unit_id;

namespace custom {
namespace renderer {

static Bytecode * bc = NULL;

static void init_defaults();

void init(Bytecode * bytecode) {
	bc = bytecode;
	init_defaults();
}

void set_shader(RefT<Shader_Asset> const & asset) {
	if (!asset.exists()) { CUSTOM_ASSERT(false, "shader doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Use_Shader);
	bc->write(asset.id);
}

void set_mesh(RefT<Mesh_Asset> const & asset) {
	if (!asset.exists()) { CUSTOM_ASSERT(false, "mesh doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Use_Mesh);
	bc->write(asset.id);
}

void set_uniform(RefT<Shader_Asset> const & shader, u32 uniform, RefT<Texture_Asset> const & texture) {
	if (!shader.exists()) { CUSTOM_ASSERT(false, "shader doesn't exist"); return; }
	if (!texture.exists()) { CUSTOM_ASSERT(false, "texture doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Allocate_Unit);
	bc->write(unit_id{texture.id, empty_asset_id});

	bc->write(custom::graphics::Instruction::Set_Uniform);
	bc->write(shader.id); bc->write(uniform);
	bc->write(custom::graphics::Data_Type::unit_id);
	bc->write((u32)1); bc->write(unit_id{texture.id, empty_asset_id});
}

void set_uniform(RefT<Shader_Asset> const & shader, u32 uniform, mat4 const & matrix) {
	if (!shader.exists()) { CUSTOM_ASSERT(false, "shader doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Set_Uniform);
	bc->write(shader.id); bc->write(uniform);
	bc->write(custom::graphics::Data_Type::mat4);
	bc->write((u32)1); bc->write(matrix);
}

void set_uniform(RefT<Shader_Asset> const & shader, u32 uniform, mat3 const & matrix) {
	if (!shader.exists()) { CUSTOM_ASSERT(false, "shader doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Set_Uniform);
	bc->write(shader.id); bc->write(uniform);
	bc->write(custom::graphics::Data_Type::mat3);
	bc->write((u32)1); bc->write(matrix);
}

void set_uniform(RefT<Shader_Asset> const & shader, u32 uniform, ivec2 const & value) {
	if (!shader.exists()) { CUSTOM_ASSERT(false, "shader doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Set_Uniform);
	bc->write(shader.id); bc->write(uniform);
	bc->write(custom::graphics::Data_Type::ivec2);
	bc->write((u32)1); bc->write(value);
}

void viewport(ivec2 const & position, ivec2 const & size) {
	bc->write(custom::graphics::Instruction::Viewport);
	bc->write(position); bc->write(size);
}

void clear(void) {
	bc->write(graphics::Instruction::Clear);
	bc->write(graphics::Clear_Flags::Color | graphics::Clear_Flags::Depth);
	// bc->write(graphics::Clear_Flags::Color | graphics::Clear_Flags::Depth | graphics::Clear_Flags::Stencil);
}

void draw(void) {
	bc->write(graphics::Instruction::Draw);
}

}}

//
// utilities
//

namespace custom {
namespace renderer {

static void init_defaults(void) {
	bc->write(graphics::Instruction::Depth_Read);
	bc->write((b8)1);

	bc->write(graphics::Instruction::Depth_Write);
	bc->write((b8)1);

	#if defined(REVERSED_Z)
		bc->write(graphics::Instruction::Depth_Range);
		bc->write(vec2{1, 0});

		bc->write(graphics::Instruction::Depth_Comparison);
		bc->write(graphics::Comparison::Greater);

		bc->write(graphics::Instruction::Depth_Clear);
		bc->write(0.0f);
	#else
		bc->write(graphics::Instruction::Depth_Range);
		bc->write(vec2{0, 1});

		bc->write(graphics::Instruction::Depth_Comparison);
		bc->write(graphics::Comparison::Less);

		bc->write(graphics::Instruction::Depth_Clear);
		bc->write(1.0f);
	#endif

	bc->write(graphics::Instruction::Color_Clear);
	bc->write(vec4{0, 0, 0, 0});

	bc->write(graphics::Instruction::Stencil_Clear);
	bc->write(0);

	bc->write(graphics::Instruction::Blend_Mode);
	bc->write(graphics::Blend_Mode::Alpha);

	bc->write(graphics::Instruction::Cull_Mode);
	bc->write(graphics::Cull_Mode::Back);

	bc->write(graphics::Instruction::Front_Face);
	bc->write(graphics::Front_Face::CCW);

	bc->write(graphics::Instruction::Clip_Control);
	bc->write(graphics::Clip_Origin::Lower_Left);
	bc->write(graphics::Clip_Depth::Zero_One);
}

}}