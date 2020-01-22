#pragma once
#include "engine/core/types.h"

#if defined(__cplusplus)
	#define BIT(T, index) static_cast<T>(static_cast<T>(1) << index)
#else
	#define BIT(T, index) (T)((T)1 << index)
#endif

//
//
//

#define BITS_ARE_SET(container, bits)\
	bits_are_set(container, (decltype(container))bits)

#define BITS_ARE_SET_IMPL(T)\
constexpr inline bool bits_are_set(T container, T bits) {\
	return (container & bits) == bits;\
}

BITS_ARE_SET_IMPL(s8)
BITS_ARE_SET_IMPL(s16)
BITS_ARE_SET_IMPL(s32)
BITS_ARE_SET_IMPL(s64)
BITS_ARE_SET_IMPL(u8)
BITS_ARE_SET_IMPL(u16)
BITS_ARE_SET_IMPL(u32)
BITS_ARE_SET_IMPL(u64)

BITS_ARE_SET_IMPL(s48)
BITS_ARE_SET_IMPL(u48)
#undef BITS_ARE_SET_IMPL

//
//
//

#define BITS_TO_ZERO(container, bits)\
	bits_to_zero(container, (decltype(container))bits)

#define BITS_TO_ZERO_IMPL(T)\
constexpr inline T bits_to_zero(T container, T bits) {\
	return container & ~bits;\
}

BITS_TO_ZERO_IMPL(s8)
BITS_TO_ZERO_IMPL(s16)
BITS_TO_ZERO_IMPL(s32)
BITS_TO_ZERO_IMPL(s64)
BITS_TO_ZERO_IMPL(u8)
BITS_TO_ZERO_IMPL(u16)
BITS_TO_ZERO_IMPL(u32)
BITS_TO_ZERO_IMPL(u64)

BITS_TO_ZERO_IMPL(s48)
BITS_TO_ZERO_IMPL(u48)
#undef BITS_TO_ZERO_IMPL

//
//
//

#define GET_BIT_AT_INDEX_IMPL(T)\
constexpr inline bool get_bit_at_index(T container, u8 index) {\
	return bits_are_set(container, BIT(T, index));\
}

GET_BIT_AT_INDEX_IMPL(s8)
GET_BIT_AT_INDEX_IMPL(s16)
GET_BIT_AT_INDEX_IMPL(s32)
GET_BIT_AT_INDEX_IMPL(s64)
GET_BIT_AT_INDEX_IMPL(u8)
GET_BIT_AT_INDEX_IMPL(u16)
GET_BIT_AT_INDEX_IMPL(u32)
GET_BIT_AT_INDEX_IMPL(u64)

GET_BIT_AT_INDEX_IMPL(s48)
GET_BIT_AT_INDEX_IMPL(u48)
#undef GET_BIT_AT_INDEX_IMPL
