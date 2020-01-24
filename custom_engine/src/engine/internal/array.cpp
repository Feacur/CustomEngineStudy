#include "custom_pch.h"

#include "engine/core/types.h"
#include "engine/impl/array.h"

namespace custom {

template struct Array<char>;

template struct Array<s8>;
template struct Array<s16>;
template struct Array<s32>;
template struct Array<s64>;

template struct Array<u8>;
template struct Array<u16>;
template struct Array<u32>;
template struct Array<u64>;

template struct Array<s48>;
template struct Array<u48>;

template struct Array<r32>;
template struct Array<r64>;

}
