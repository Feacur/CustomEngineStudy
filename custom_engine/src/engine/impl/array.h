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

template<typename T>
void Array<T>::add() {
	CUSTOM_ASSERT(count < capacity, "count exceeds capacity");
	++count;
}

template<typename T>
void Array<T>::add(T const & value) {
	u32 i = count;
	add();
	data[i] = value;
}

template<typename T>
void Array<T>::insert(u32 i) {
	CUSTOM_ASSERT(count < capacity, "count exceeds capacity");
	CUSTOM_ASSERT(i <= count, "index exceeds count");
	memcpy(data + i + 1, data + i, (count - i)  * sizeof(T));
	++count;

}

template<typename T>
void Array<T>::insert(u32 i, T const & value) {
	insert();
	data[i] = value;
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

template<typename T, u16 capacity>
Array_Fixed<T, capacity>::Array_Fixed(u16 count)
	: count(source.count)
	// , bytes() // @Note: clear to zero
{ }

template<typename T, u16 capacity>
Array_Fixed<T, capacity>::Array_Fixed(Array_Fixed<T, capacity> const & source)
	: count(source.count)
{
	CUSTOM_ASSERT(bytes != source.bytes, "assigning itself");
	memcpy(bytes, source.bytes, sizeof(bytes));
}

template<typename T, u16 capacity>
inline Array_Fixed<T, capacity> & Array_Fixed<T, capacity>::operator=(Array_Fixed<T, capacity> const & source)
{
	CUSTOM_ASSERT(bytes != source.bytes, "assigning itself");
	memcpy(bytes, source.bytes, sizeof(bytes));
	count = source.count;
	return *this;
}

template<typename T, u16 capacity>
inline T const & Array_Fixed<T, capacity>::operator[](u16 i) const {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T, u16 capacity>
inline T & Array_Fixed<T, capacity>::operator[](u16 i) {
	CUSTOM_ASSERT(i < capacity, "index exceeds capacity");
	return data[i];
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::add() {
	CUSTOM_ASSERT(count < capacity, "count exceeds capacity");
	++count;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::add(T const & value) {
	u16 i = count;
	add();
	data[i] = value;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::insert(u16 i) {
	CUSTOM_ASSERT(count < capacity, "count exceeds capacity");
	CUSTOM_ASSERT(i <= count, "index exceeds count");
	memcpy(data + i + 1, data + i, (count - i)  * sizeof(T));
	++count;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::insert(u16 i, T const & value) {
	insert();
	data[i] = value;
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::remove(u16 i) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	CUSTOM_ASSERT(i < count, "index exceeds count");
	--count;
	if (i != count) {
		data[i] = data[count];
	}
}

template<typename T, u16 capacity>
void Array_Fixed<T, capacity>::remove_ordered(u16 i) {
	CUSTOM_ASSERT(count > 0, "count is zero");
	CUSTOM_ASSERT(i < count, "index exceeds count");
	--count;
	memcpy(data + i, data + i + 1, (count - i) * sizeof(T));
}

}
