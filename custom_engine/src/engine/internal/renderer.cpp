#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/renderer.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/array.h"
#include "engine/impl/bytecode.h"

namespace custom {
namespace renderer {

static Bytecode * bc = NULL;

static void init_defaults();

void init(Bytecode * bytecode) {
	bc = bytecode;
	init_defaults();
}

void clear() {
	bc->write(graphics::Instruction::Clear);
	bc->write(graphics::Clear_Flags::Color | graphics::Clear_Flags::Depth);
}

void viewport(ivec2 const & position, ivec2 const & size) {
	bc->write(custom::graphics::Instruction::Viewport);
	bc->write(position); bc->write(size);
}

}}

//
// utilities
//

namespace custom {
namespace renderer {

static void init_defaults() {
	bc->write(graphics::Instruction::Depth_Read);
	bc->write((b8)1);

	bc->write(graphics::Instruction::Depth_Write);
	bc->write((b8)1);

	bc->write(graphics::Instruction::Depth_Comparison);
	bc->write(graphics::Comparison::Less);

	bc->write(graphics::Instruction::Blend_Mode);
	bc->write(graphics::Blend_Mode::Alpha);

	bc->write(graphics::Instruction::Cull_Mode);
	bc->write(graphics::Cull_Mode::Back);
}

}}