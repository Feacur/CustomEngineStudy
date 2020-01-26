#pragma once
#include "engine/core/collection_types.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

#include <utility>

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

#if defined(CUSTOM_ARRAY_POD) // constructor
	#if defined(CUSTOM_ARRAY_WARN) // constructor
	template<typename T>
	Array<T>::Array(Array<T> const & source) {
		CUSTOM_ASSERT(false, "ERROR! copying an array");
	}
	template<typename T>
	Array<T>::Array(Array<T> && source) {
		CUSTOM_ASSERT(false, "ERROR! moving an array");
	}
	#endif // defined(CUSTOM_ARRAY_WARN) // constructor
#else
template<typename T>
Array<T>::Array(Array<T> const & source)
	: data(NULL)
	, capacity(0)
	, count(source.count)
{
	CUSTOM_ASSERT(data != source.data, "moving itself");
	CUSTOM_MESSAGE("WARNING! copying an array of size %d", source.count);
	set_capacity(source.count);
	memcpy(data, source.data, source.count * sizeof(T));
}

template<typename T>
Array<T>::Array(Array<T> && source)
	: data(source.data)
	, capacity(source.count)
	, count(source.count)
{
	CUSTOM_ASSERT(data != source.data, "moving itself");
	CUSTOM_MESSAGE("WARNING! moving an array");
	source.data = NULL;
	source.capacity = NULL;
	source.count = NULL;
}
#endif // defined(CUSTOM_ARRAY_POD) // constructor

template<typename T>
Array<T>::~Array() {
	free(data);
	data = NULL;
	capacity = count = 0;
}

#if defined(CUSTOM_ARRAY_POD) // operator=
	#if defined(CUSTOM_ARRAY_WARN) // operator=
	template<typename T>
	Array<T> & Array<T>::operator=(Array<T> const & source) {
		CUSTOM_ASSERT(false, "ERROR! copying an array");
		return *this;
	}
	template<typename T>
	Array<T> & Array<T>::operator=(Array<T> && source) {
		CUSTOM_ASSERT(false, "ERROR! moving an array");
		return *this;
	}
	#endif // defined(CUSTOM_ARRAY_WARN) // operator=
#else
template<typename T>
Array<T> & Array<T>::operator=(Array<T> const & source) {
	CUSTOM_ASSERT(data != source.data, "copying itself");
	CUSTOM_MESSAGE("WARNING! copying an array of size %d", source.count);
	set_capacity(source.count);
	memcpy(data, source.data, source.count * sizeof(T));
	count = source.count;
	return *this;
}

template<typename T>
Array<T> & Array<T>::operator=(Array<T> && source) {
	CUSTOM_ASSERT(data != source.data, "moving itself");
	CUSTOM_MESSAGE("WARNING! moving an array");
	free(data);
	data = NULL;
	capacity = count = 0;

	data = source.data;
	capacity = source.capacity;
	count = source.count;

	source.data = NULL;
	source.capacity = NULL;
	source.count = NULL;

	return *this;
}
#endif // defined(CUSTOM_ARRAY_POD) // operator=

template<typename T>
inline T const & Array<T>::operator[](u32 i) const {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T>
inline T & Array<T>::operator[](u32 i) {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T>
void Array<T>::set_capacity(u32 amount) {
	if (data) {
		if (amount) {
			data = (T *)realloc(data, amount * sizeof(T));
			// if (amount > capacity) { // @Note: clear to zero
			// 	memset(data + capacity, 0, (amount - capacity) * sizeof(T));
			// }
		}
		else {
			free(data);
		}
	}
	else {
		data = (T *)malloc(amount * sizeof(T));
		// data = (T *)calloc(amount, sizeof(T)); // @Note: clear to zero
	}
	capacity = amount;
	if (count > capacity) {
		count = capacity;
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

// template<typename T>
// void Array<T>::push_move(T && value) {
// 	ensure_capacity(count + 1);
// 	data[count++] = std::move(value);
// }

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

// template<typename T>
// void Array<T>::insert_move(u32 i, T && value) {
// 	insert(i);
// 	data[i] = std::move(value);
// }

template<typename T>
void Array<T>::pop() {
	CUSTOM_ASSERT(count > 0, "count is zero");
	--count;
}

template<typename T>
void Array<T>::remove(u32 i) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	CUSTOM_ASSERT(i < count, "index exceeds count");
	--count;
	if (i != count) {
		data[i] = data[count];
	}
}

template<typename T>
void Array<T>::remove_ordered(u32 i) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	CUSTOM_ASSERT(i < count, "index exceeds count");
	--count;
	memcpy(data + i, data + i + 1, (count - i) * sizeof(T));
}

}
