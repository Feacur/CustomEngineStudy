#pragma once
#include "engine/core/types.h"
#include "engine/api/internal/strings_storage.h"

namespace custom {
	// @Forward
	template<typename T> struct Array;
}

namespace custom {
namespace file {
extern Strings_Storage modified;

u64 get_time(cstring path);
void read(cstring path, Array<u8> & buffer);

void watch_init(cstring path, bool subtree);
void watch_update(void);
void watch_shutdown(void);

}}
