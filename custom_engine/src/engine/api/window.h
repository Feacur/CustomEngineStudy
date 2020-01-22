#pragma once
#include "engine/core/types.h"

namespace custom {

struct Graphics_Context;

// @Note: a C++ class - probably better using std::shared_ptr<T>
//        otherwise it's just "risky" to give out an instance
//        and then have it copy-destructed or something
struct Window
{
	static bool should_close;

	Window();
	~Window();

	void init_context();

	void update();

	void set_vsync(s32 value);
	bool is_vsync() const;
	void set_header(cstring value) const;

private:
	uptr m_handle;
	Graphics_Context * m_graphics_context;
};

}
