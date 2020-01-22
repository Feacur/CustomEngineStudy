#pragma once
#include "engine/core/collection_types.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

namespace custom {
//
// language facilities
//
template<typename T>
Array<T>::Array(u16 capacity, u16 count)
	: data(NULL)
	, capacity(0)
	, count(count)
{
	CUSTOM_ASSERT(count <= capacity, "count exceeds capacity");
	if (capacity) {
		resize(capacity);
	}
}
	
template<typename T>
Array<T>::~Array()
{
	free(data);
	data = NULL;
	capacity = count = 0;
}

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

//
// API
//
template<typename T>
void Array<T>::resize(u32 amount)
{
	if (data) {
		data = (T *)realloc(data, amount * sizeof(T));
		// if (amount > capacity) { // @Note: clear to zero
		// 	memset(data + capacity, 0, (amount - capacity) * sizeof(T));
		// }
	}
	else {
		data = (T *)malloc(amount * sizeof(T));
		// data = (T *)calloc(amount, sizeof(T)); // @Note: clear to zero
	}
	capacity = amount;
}

//
// language facilities
//
template<typename T, u16 capacity>
Array_Fixed<T, capacity>::Array_Fixed(u16 count)
	: count(source.count)
	// , bytes() // @Note: clear to zero
{ }

template<typename T, u16 capacity>
Array_Fixed<T, capacity>::Array_Fixed(Array_Fixed<T, capacity> const & source)
	: count(source.count)
{
	memcpy(bytes, source.bytes, sizeof(bytes));
}

template<typename T, u16 capacity>
inline Array_Fixed<T, capacity> & Array_Fixed<T, capacity>::operator=(Array_Fixed<T, capacity> const & source)
{
	memcpy(bytes, source.bytes, sizeof(bytes));
	count = source.count;
	return *this;
}

template<typename T, u16 capacity>
inline T const & Array_Fixed<T, capacity>::operator[](u32 i) const {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T, u16 capacity>
inline T & Array_Fixed<T, capacity>::operator[](u32 i) {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

//
// API
//

}
