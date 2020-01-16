#pragma once
#include "engine/core/code.h"

namespace custom
{
	class Window
	{
	public:
		Window();
		~Window();

		void update();
		void set_header(cstring value);

		u64 get_handle() { return handle; }
		u64 get_display() { return display; }
		u64 get_graphics() { return graphics; }

	private:
		u64 handle;
		u64 display;
		u64 graphics;
	};
}
