#pragma once
#include "engine/core/types.h"

namespace custom {
namespace unicode {

typedef char const * utf8;

u32 decode(utf8 value);
utf8 next(utf8 value);
u32 count(utf8 value);

}}
