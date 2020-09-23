#pragma once
#include "engine/core/types.h"

#define COLLECTION_COPY_WARNING

#if defined(CUSTOM_SHIPPING)
	#undef COLLECTION_COPY_WARNING
#endif

// @Note: this code implies POD types
//        all of the T, Array<T>, Array_Fixed<T, N>

namespace custom {

constexpr u32 const empty_index_sb = UINT32_MAX;

struct SBuffer
{
	void * data;
	u32 capacity, count;
	u32 item_size;

	SBuffer(u32 item_size, u32 capacity = 0, u32 count = 0);
	#if defined(COLLECTION_COPY_WARNING)
		SBuffer(SBuffer const & source);
	#endif
	~SBuffer(void);

	#if defined(COLLECTION_COPY_WARNING)
		SBuffer & operator=(SBuffer const & source);
	#endif

	void set_capacity(u32 number);
	void ensure_capacity(u32 number);

	void push_range_void(u32 number);
	void pop_range_void(u32 number);
};

}
