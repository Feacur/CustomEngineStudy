#pragma once
#include "engine/core/code.h"

namespace custom
{
	class Rendering_Context
	{
	public:
		virtual ~Rendering_Context() = default;
		virtual void swap_interval(s32 value) = 0;
		virtual void swap_buffers() = 0;
	};
}
