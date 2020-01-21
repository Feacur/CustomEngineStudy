#pragma once
#include "engine/core/code.h"

namespace custom
{
	class Graphics_Context;
	struct Context_Settings;
	struct Pixel_Format;

	// @Note: a C++ class - probably better using std::shared_ptr<T>
	//        otherwise it's just "risky" to give out an instance
	//        and then have it copy-destructed or something
	class Window
	{
	public:
		static bool should_close;

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
		Graphics_Context * m_graphics_context;
	};
}
