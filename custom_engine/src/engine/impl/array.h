#pragma once
#include "engine/core/collection_types.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

namespace custom {

template<typename T>
Array<T>::Array(u32 capacity, u32 count)
	: data(NULL)
	, capacity(0)
	, count(count)
{
	CUSTOM_ASSERT(count <= capacity, "count exceeds capacity");
	if (capacity) {
		set_capacity(capacity);
	}
}

#if !defined(CUSTOM_SHIPPING)
	template<typename T>
	Array<T>::Array(Array<T> const & source) {
		CUSTOM_ASSERT(false, "ERROR! trying to copy an array");
	}
#endif

template<typename T>
Array<T>::~Array() {
	free(data); data = NULL;
	capacity = count = 0;
}

#if !defined(CUSTOM_SHIPPING)
	template<typename T>
	Array<T> & Array<T>::operator=(Array<T> const & source) {
		CUSTOM_ASSERT(false, "ERROR! trying to copy an array");
		return *this;
	}
#endif

template<typename T>
inline T const & Array<T>::operator[](u32 i) const {
	// @Todo: warn of reading beyond count?
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T>
inline T & Array<T>::operator[](u32 i) {
	// @Todo: warn of reading beyond count?
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T>
void Array<T>::set_capacity(u32 amount) {
	if (!amount) {
		free(data); data = NULL;
		capacity = count = 0;
		return;
	}

	if (!data) {
		data = (T *)calloc(amount, sizeof(T));
		capacity = amount;
		return;
	}

	void * new_buffer = realloc(data, amount * sizeof(T));
	CUSTOM_ASSERT(new_buffer, "failed to allocate memory of %zd bytes", amount * sizeof(T));

	if (new_buffer) {
		if (amount > capacity) {
			memset((T *)new_buffer + capacity, 0, (amount - capacity) * sizeof(T));
		}
		data = (T *)new_buffer;
		capacity = amount;
		if (count > capacity) {
			count = capacity;
		}
	}
}

template<typename T>
void Array<T>::ensure_capacity(u32 amount) {
	if (amount > capacity) {
		// @Note: might require mul_div(...)
		set_capacity((amount + 1) * 3 / 2);
	}
}

template<typename T>
void Array<T>::push() {
	ensure_capacity(++count);
}

template<typename T>
void Array<T>::push(T const & value) {
	ensure_capacity(count + 1);
	data[count++] = value;
}

template<typename T>
void Array<T>::push_range(u32 amount) {
	ensure_capacity(count += amount);
}

template<typename T>
void Array<T>::push_range(T const * values, u32 amount) {
	ensure_capacity(count + amount);
	memcpy(data + count, values, amount * sizeof(T));
	count += amount;
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
void Array<T>::pop() {
	CUSTOM_ASSERT(count > 0, "count is zero");
	--count;
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
