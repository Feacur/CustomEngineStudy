#pragma once
#include "engine/core/types.h"

namespace custom {
namespace unicode {

typedef char const * utf8;

u32 decode(utf8 buffer);
utf8 next(utf8 buffer);
u32 count(utf8 buffer);

}}
