#pragma once
#include "engine/core/types.h"

namespace custom {
namespace system {

// @Note: [ ] a singleton?           - is error-prone due to copy-construction
//        [ ] a static class/struct? - is merely a namespace in the end
//        [ ] a namespace?           - is nice to scope some data
//        [x] a static instance?     - is a scoped data holder

// @Note: a static instance; System::instance would be the same data-wise
extern bool should_close;

void init(void);
void update(void);
u64  get_time(void);

}}
