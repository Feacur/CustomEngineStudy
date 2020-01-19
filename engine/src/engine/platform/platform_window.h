#pragma once
#include "engine/core/code.h"

namespace custom
{
	class Rendering_Context;
	struct Context_Settings;
	struct Pixel_Format;

	class Window
	{
	public:
		bool should_close;

	public:
		Window();
		~Window();

		void init_context(Context_Settings * settings, Pixel_Format * hint);

		void update();

		void set_vsync(s32 value);
		bool is_vsync() const;
		void set_header(cstring value) const;

	private:
		uptr m_handle;
		Rendering_Context * m_rendering_context;
	};
}
