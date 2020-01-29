#pragma once
#include "engine/core/types.h"

namespace custom {

template<typename T> struct Array;

void file_read(cstring path, Array<u8> & buffer);

}
