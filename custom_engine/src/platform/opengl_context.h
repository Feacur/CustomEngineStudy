#pragma once
#include "platform/graphics_context.h"

namespace custom {

struct Context_Settings;
struct Pixel_Format;

struct Opengl_Context : public Graphics_Context
{
	Opengl_Context(uptr hdc);
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
