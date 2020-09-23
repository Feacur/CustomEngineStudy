#pragma once
#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/debug/log.h"

namespace custom {

template<typename T>
Array<T>::Array(u32 capacity, u32 count)
	: data(NULL)
	, capacity(capacity)
	, count(count)
{
	CUSTOM_ASSERT(count <= capacity, "count exceeds capacity");
	if (capacity) {
		set_capacity(capacity);
	}
}

#if defined(COLLECTION_COPY_WARNING)
	template<typename T>
	Array<T>::Array(Array const & source) {
		CUSTOM_ASSERT(false, "ERROR! trying to copy an array");
	}
#endif

template<typename T>
Array<T>::~Array(void) {
	free(data); data = NULL;
	capacity = count = 0;
}

#if defined(COLLECTION_COPY_WARNING)
	template<typename T>
	Array<T> & Array<T>::operator=(Array const & source) {
		CUSTOM_ASSERT(false, "ERROR! trying to copy an array");
		return *this;
	}
#endif

template<typename T>
inline T const & Array<T>::operator[](u32 i) const {
	CUSTOM_ASSERT(i < count, "index exceeds capacity");
	// CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T>
inline T & Array<T>::operator[](u32 i) {
	CUSTOM_ASSERT(i < count, "index exceeds capacity");
	// CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T>
inline T const & Array<T>::get(u32 i) const {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T>
inline T & Array<T>::get(u32 i) {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T>
void Array<T>::set_capacity(u32 number) {
	if (!number) {
		free(data); data = NULL;
		capacity = count = 0;
		return;
	}

	if (!data) {
		data = (T *)malloc(number * sizeof(T));
		capacity = number;
		return;
	}

	void * new_buffer = realloc(data, number * sizeof(T));
	CUSTOM_ASSERT(new_buffer, "failed to allocate memory of %zd bytes", number * sizeof(T));

	if (new_buffer) {
		data = (T *)new_buffer;
		capacity = number;
		if (count > capacity) {
			count = capacity;
		}
	}
}

template<typename T>
void Array<T>::ensure_capacity(u32 number) {
	if (number > capacity) {
		// @Note: might require mul_div(...)
		set_capacity((number + 1) * 3 / 2);
	}
}

template<typename T>
void Array<T>::push(void) {
	ensure_capacity(++count);
}

template<typename T>
void Array<T>::push(T const & value) {
	ensure_capacity(count + 1);
	data[count++] = value;
}

template<typename T>
void Array<T>::push_range(u32 number) {
	ensure_capacity(count += number);
}

template<typename T>
void Array<T>::push_range(T const * values, u32 number) {
	ensure_capacity(count + number);
	memcpy(data + count, values, number * sizeof(T));
	count += number;
}

template<typename T>
void Array<T>::insert(u32 i) {
	CUSTOM_ASSERT(i <= count, "index exceeds count");
	ensure_capacity(++count);
	memcpy(data + i + 1, data + i, (count - 1 - i)  * sizeof(T));
}

template<typename T>
void Array<T>::insert(u32 i, T const & value) {
	insert(i);
	data[i] = value;
}

template<typename T>
void Array<T>::pop(void) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	--count;
}

template<typename T>
void Array<T>::pop_range(u32 number) {
	CUSTOM_ASSERT(count >= number, "count is zero");
	count -= number;
}

template<typename T>
void Array<T>::remove_at(u32 i) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	CUSTOM_ASSERT(i < count, "index exceeds count");
	--count;
	if (i != count) {
		data[i] = data[count];
	}
}

template<typename T>
void Array<T>::remove_at_ordered(u32 i) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	CUSTOM_ASSERT(i < count, "index exceeds count");
	--count;
	memcpy(data + i, data + i + 1, (count - i) * sizeof(T));
}

}
