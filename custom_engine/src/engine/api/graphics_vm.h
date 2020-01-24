#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace custom {

enum struct Graphics_Instruction : u8
{
	None,
	//
	Viewport, // x, y, width, height
	//
	Allocate_Shader,
	Allocate_Texture,
	Allocate_Mesh,
	//
	Free_Shader,
	Free_Texture,
	Free_Mesh,
	//
	Use_Shader,
	Use_Texture,
	Use_Mesh,
	//
	Set_Uniform,
	//
	Draw,
	Overlay,
	//
	Print_Pointer,
	Print_Inline,
	Last,
};
UNDERLYING_TYPE_META(Graphics_Instruction, u8)
IS_ENUM_META(Graphics_Instruction)

struct Command_Buffer;

struct Graphics_VM
{
	Graphics_VM();
	~Graphics_VM();

	void render(Command_Buffer const & command_buffer);
};

}
