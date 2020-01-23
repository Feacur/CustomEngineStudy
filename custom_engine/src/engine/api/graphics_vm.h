#pragma once
#include "engine/api/command_buffer.h"

namespace custom {

enum struct Graphics_Instruction : u8
{
	None,
	Print_Pointer,
	Print_Inline,
	Last,
};

struct Graphics_VM
{
	Graphics_VM();
	~Graphics_VM();

	void render(Command_Buffer & command_buffer);
};

}
