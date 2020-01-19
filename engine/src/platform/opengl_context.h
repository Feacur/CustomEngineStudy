#pragma once
#include "engine/core/code.h"
#include "engine/platform/rendering_context.h"

namespace custom
{
	class Opengl_Context : public Rendering_Context
	{
	public:
		Opengl_Context(uptr hdc);
		~Opengl_Context() override;

		void swap_interval(s32 value) override;
		void swap_buffers() override;

	private:
		uptr m_hdc;
		uptr m_hrc;
	};
}
