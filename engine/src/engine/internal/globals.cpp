#include "custom_pch.h"
#include "engine/core/code.h"
#include "engine/api/system.h"
#include "engine/api/timer.h"
#include "engine/api/window.h"

namespace custom
{
	System system = {};

	Timer timer = {};
	u64 millisecond = 1000;
	u64 microsecond = 1000000;
	u64 nanosecond  = 1000000000;

	bool Window::should_close = false;
}
