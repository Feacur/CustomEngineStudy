#pragma once
#include "engine/core/code.h"

namespace custom
{
	class Opengl_Context
	{
	public:
		static void init(uptr graphics, uptr dummy_graphics);
		static void shutdown();

		static void swap_interval(uptr display, uptr graphics, s32 value);
		static void swap_buffers(uptr display, uptr graphics);
	};
}
