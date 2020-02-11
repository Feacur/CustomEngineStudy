#pragma once
#include "engine/core/types.h"

namespace custom {
namespace timer {

// @Note: [ ] a singleton?           - is error-prone due to copy-construction
//        [ ] a static class/struct? - is merely a namespace in the end
//        [ ] a namespace?           - is nice to scope some data
//        [x] a static instance?     - is a scoped data holder

// @Note: a static instance; Timer::instance would be the same data-wise
extern u64 ticks_per_second;
extern u64 frame_start_ticks;

void init(void);
u64  snapshot(void);
u64  wait_next_frame(u64 duration, u64 precision);

static constexpr u64 const millisecond = 1000;
static constexpr u64 const microsecond = 1000000;
static constexpr u64 const nanosecond  = 1000000000;

}}
