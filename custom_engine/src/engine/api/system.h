#pragma once
#include "engine/core/types.h"

namespace custom {

// @Note: [ ] a singleton?           - is error-prone due to copy-construction
//        [ ] a static class/struct? - is merely a namespace in the end
//        [ ] a namespace?           - is nice to scope some data
//        [x] a static instance?     - is a scoped data holder
struct System {
	bool should_close;
};

// @Note: a static instance; Timer::instance would be the same data-wise
extern System system;

void system_init();
void system_update();
u64  system_get_time();

}
