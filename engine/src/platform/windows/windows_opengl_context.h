#pragma once
#include "engine/core/code.h"

namespace custom
{
	class Opengl_Context
	{
	public:
		static void init(u64 graphics);
		static void shutdown();
	};
}
