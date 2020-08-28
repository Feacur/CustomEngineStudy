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

}}

namespace custom {
namespace graphics {

template<typename T> constexpr static inline Data_Type get_data_type(void) { return Data_Type::None; }
#define DATA_TYPE_IMPL(T) template<> constexpr inline Data_Type get_data_type<T>(void) { return Data_Type::T; }
#include "engine/registry_impl/data_type.h"

extern u16 get_type_size(Data_Type value);

}}

//
//
//

namespace custom {
namespace renderer {

graphics::unit_id make_unit(RefT<Texture_Asset> const & asset) {
	if (!asset.exists()) {
		CUSTOM_ASSERT(false, "texture asset doesn't exist");
		return {custom::empty_ref.id, custom::empty_ref.id};
	}
	// @Todo: make use of samplers; automate?
	unit_id unit = {asset.id, custom::empty_ref.id};
	bc->write(custom::graphics::Instruction::Allocate_Unit);
	bc->write(unit);
	return unit;
}

}}

//
//
//

namespace custom {
namespace renderer {

void set_shader(RefT<Shader_Asset> const & asset) {
	if (!asset.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Use_Shader);
	bc->write(asset.id);
}

void set_mesh(RefT<Mesh_Asset> const & asset) {
	if (!asset.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Use_Mesh);
	bc->write(asset.id);
}

void set_uniform_bytes(RefT<Shader_Asset> const & shader, u32 uniform, u8 const * data, u32 count, graphics::Data_Type type) {
	if (!shader.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }
	bc->write(custom::graphics::Instruction::Set_Uniform);
	bc->write(shader.id); bc->write(uniform);
	bc->write(type); bc->write(count);
	bc->write(data, count * custom::graphics::get_type_size(type));
}

void viewport(ivec2 const & position, ivec2 const & size) {
	bc->write(custom::graphics::Instruction::Viewport);
	bc->write(position); bc->write(size);
}

void clear(graphics::Clear_Flag flags) {
	if (flags == graphics::Clear_Flag::None) { return; }
	bc->write(graphics::Instruction::Clear);
	bc->write(flags);
}

void draw(void) {
	bc->write(graphics::Instruction::Draw);
}

}}

//
//
//

namespace custom {
namespace renderer {

#define DATA_TYPE_IMPL(T) template<> void set_uniform<T>(RefT<Shader_Asset> const & shader, u32 uniform, T const & value) {\
	set_uniform_bytes(\
		shader, uniform,\
		(u8 *)&value, 1, graphics::get_data_type<T>()\
	);\
}\

#include "engine/registry_impl/data_type.h"

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