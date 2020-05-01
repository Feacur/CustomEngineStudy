#pragma once
#include "engine/core/types.h"

namespace custom {
template<typename T> struct Array;
}

namespace custom {
namespace file {

void read(cstring path, Array<u8> & buffer);

}}
