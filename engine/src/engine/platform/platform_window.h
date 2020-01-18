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

		uptr get_handle() { return handle; }
		uptr get_display() { return display; }
		uptr get_graphics() { return graphics; }

	private:
		uptr handle;
		uptr display;
		uptr graphics;
	};
}
