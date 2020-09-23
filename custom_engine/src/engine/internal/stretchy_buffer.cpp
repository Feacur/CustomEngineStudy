#pragma once
#include "engine/core/code.h"
#include "engine/core/stretchy_buffer.h"
#include "engine/debug/log.h"

namespace custom {

SBuffer::SBuffer(u32 item_size, u32 capacity, u32 count)
	: data(NULL)
	, capacity(capacity)
	, count(count)
	, item_size(item_size)
{
	CUSTOM_ASSERT(count <= capacity, "count exceeds capacity");
	if (capacity) {
		set_capacity(capacity);
	}
}

#if defined(COLLECTION_COPY_WARNING)
	SBuffer::SBuffer(SBuffer const & source) : item_size(source.item_size) {
		CUSTOM_ASSERT(false, "ERROR! trying to copy an SBuffer");
	}
#endif

SBuffer::~SBuffer(void) {
	free(data); data = NULL;
	capacity = count = 0;
}

#if defined(COLLECTION_COPY_WARNING)
	SBuffer & SBuffer::operator=(SBuffer const & source) {
		CUSTOM_ASSERT(false, "ERROR! trying to copy an SBuffer");
		return *this;
	}
#endif

void SBuffer::set_capacity(u32 number) {
	if (!number) {
		free(data); data = NULL;
		capacity = count = 0;
		return;
	}

	if (!data) {
		data = malloc(number * item_size);
		capacity = number;
		return;
	}

	void * new_buffer = realloc(data, number * item_size);
	CUSTOM_ASSERT(new_buffer, "failed to allocate memory of %d bytes", number * item_size);

	if (new_buffer) {
		data = new_buffer;
		capacity = number;
		if (count > capacity) {
			count = capacity;
		}
	}
}


void SBuffer::ensure_capacity(u32 number) {
	if (number > capacity) {
		// @Note: might require mul_div(...)
		set_capacity((number + 1) * 3 / 2);
	}
}

void SBuffer::push_range_void(u32 number) {
	ensure_capacity(count += number);
}

void SBuffer::pop_range_void(u32 number) {
	CUSTOM_ASSERT(count >= number, "count is zero");
	count -= number;
}

}
