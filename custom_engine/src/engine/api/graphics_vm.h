#pragma once
#include "engine/api/command_buffer.h"

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
	Draw_Mesh,
	Draw_Overlay,
	//
	Print_Pointer,
	Print_Inline,
	Last,
};

struct Graphics_VM
{
	Graphics_VM();
	~Graphics_VM();

	void render(Command_Buffer const & command_buffer);
};

}
