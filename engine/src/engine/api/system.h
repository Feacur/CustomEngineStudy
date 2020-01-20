#pragma once
#include "engine/core/code.h"

namespace custom
{
	// @Note: a singleton? a static instance? a static class/struct? a namespace?
	struct System {
		bool should_close;
	};
	extern System system;

	void system_init();
	void system_update();
	u64  system_get_time();
}
