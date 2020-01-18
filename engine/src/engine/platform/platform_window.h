#pragma once
#include "engine/core/code.h"

namespace custom
{
	class Rendering_Context;
 
	class Window
	{
	public:
		bool should_close;

	public:
		Window(bool is_dummy);
		~Window();

		void init_context();

		void update();

		void set_header(cstring value);

		uptr get_handle() { return m_handle; }

	private:
		uptr m_handle;
		Rendering_Context * m_rendering_context;
	};
}
