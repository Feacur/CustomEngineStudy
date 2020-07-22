#pragma once
#include "engine/core/collection_types.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

namespace custom {

template<typename T, u16 capacity>
Array_Fixed<T, capacity>::Array_Fixed(u16 count)
	: count(count)
	, data()
{ }

#if !defined(COLLECTION_COPY_WARNING)
	template<typename T, u16 capacity>
	Array_Fixed<T, capacity>::Array_Fixed(Array_Fixed<T, capacity> const & source) {
		CUSTOM_ASSERT(false, "ERROR! trying to copy a fixed array");
	}
#endif

#if !defined(COLLECTION_COPY_WARNING)
	template<typename T, u16 capacity>
	inline Array_Fixed<T, capacity> & Array_Fixed<T, capacity>::operator=(Array_Fixed<T, capacity> const & source) {
		CUSTOM_ASSERT(false, "ERROR! trying to copy a fixed array");
		return *this;
	}
#endif

template<typename T, u16 capacity>
inline T const & Array_Fixed<T, capacity>::operator[](u16 i) const {
	CUSTOM_ASSERT(i < count, "index exceeds capacity");
	// CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T, u16 capacity>
inline T & Array_Fixed<T, capacity>::operator[](u16 i) {
	CUSTOM_ASSERT(i < count, "index exceeds capacity");
	// CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T, u16 capacity>
inline T const & Array_Fixed<T, capacity>::get(u16 i) const {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T, u16 capacity>
inline T & Array_Fixed<T, capacity>::get(u16 i) {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::push() {
	CUSTOM_ASSERT(count < capacity, "count exceeds capacity");
	++count;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::push(T const & value) {
	data[count++] = value;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::push_range(u16 number) {
	CUSTOM_ASSERT(count + number <= capacity, "count exceeds capacity");
	count += number;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::push_range(T const * values, u16 number) {
	CUSTOM_ASSERT(count + number <= capacity, "count exceeds capacity");
	memcpy(data + count, values, number * sizeof(T));
	count += number;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::insert(u16 i) {
	CUSTOM_ASSERT(count < capacity, "count exceeds capacity");
	CUSTOM_ASSERT(i <= count, "index exceeds count");
	++count;
	memcpy(data + i + 1, data + i, (count - 1 - i)  * sizeof(T));
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::insert(u16 i, T const & value) {
	insert(i);
	data[i] = value;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::pop() {
	CUSTOM_ASSERT(count > 0, "count is zero");
	--count;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::remove_at(u16 i) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	CUSTOM_ASSERT(i < count, "index exceeds count");
	--count;
	if (i != count) {
		data[i] = data[count];
	}
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::remove_at_ordered(u16 i) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	CUSTOM_ASSERT(i < count, "index exceeds count");
	--count;
	memcpy(data + i, data + i + 1, (count - i) * sizeof(T));
}

}
