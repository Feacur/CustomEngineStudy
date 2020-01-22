#pragma once
#include "engine/api/command_buffer.h"

namespace custom {

struct Graphics_VM
{
	Graphics_VM();
	~Graphics_VM();

	void render(Command_Buffer & command_buffer);
};

}
