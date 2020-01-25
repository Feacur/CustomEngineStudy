#include "custom_pch.h"
#include "engine/api/command_buffer.h"

namespace custom {

Command_Buffer::Command_Buffer()
	: bytecode()
	, offset(0)
{

}

Command_Buffer::~Command_Buffer() = default;

}
