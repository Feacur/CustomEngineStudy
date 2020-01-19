#pragma once
#include "engine/core/code.h"
#include "graphics_context.h"

namespace custom
{
	struct Context_Settings;
	struct Pixel_Format;

	class Opengl_Context : public Graphics_Context
	{
	public:
		Opengl_Context(uptr hdc, Context_Settings * settings, Pixel_Format * hint);
		~Opengl_Context() override;

		void set_vsync(s32 value) override;
		bool is_vsync() const override { return m_is_vsync; }
		void swap_buffers() override;

	private:
		uptr m_hdc;
		uptr m_hrc;
		bool m_is_vsync;
	};
}
