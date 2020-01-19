#pragma once
#include "engine/core/code.h"

#define CUSTOM_WINDOW_CLASS_NAME "custom engine"

namespace custom
{
	class Graphics_Context
	{
	public:
		virtual ~Graphics_Context() = default;
		virtual void set_vsync(s32 value) = 0;
		virtual bool is_vsync() const = 0;
		virtual void swap_buffers() = 0;
	};
}
