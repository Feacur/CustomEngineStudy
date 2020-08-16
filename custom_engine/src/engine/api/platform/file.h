#pragma once
#include "engine/core/types.h"

namespace custom {
	// @Forward
	template<typename T> struct Array;
}

namespace custom {
namespace file {

bool exists(cstring path);
void read(cstring path, Array<u8> & buffer);

}}
