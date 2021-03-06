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

void init(void);
void shutdown(void);
u64  get_ticks(void);
void idle_till_next_frame(u64 frame_start_ticks, u64 duration, u64 precision);
void sleep_till_next_frame(u64 frame_start_ticks, u64 duration, u64 precision);

constexpr static u64 const millisecond = 1000;
constexpr static u64 const microsecond = 1000000;
constexpr static u64 const nanosecond  = 1000000000;

}}
