#pragma once
#include "engine/core/code.h"

namespace custom
{
	class Opengl_Context
	{
	public:
		static void init(uptr graphics, uptr dummy_graphics);
		static void shutdown();
	};
}
