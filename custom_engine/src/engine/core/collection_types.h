#pragma once
#include "engine/core/types.h"

namespace custom {

template<typename T>
struct Array
{
	T * data;
	u32 capacity, count;

	Array(u16 capacity = 0, u16 count = 0);
	Array(Array const & source) = delete;
	~Array();

	Array & operator=(Array const & source) = delete;
	T const & operator[](u32 i) const;
	T & operator[](u32 i);

	void resize(u32 amount);
};

template<typename T, u16 capacity>
struct Array_Fixed
{
	union
	{
		T data[capacity];
		u8 bytes[capacity * sizeof(T)];
	};
	u16 count;

	Array_Fixed(u16 count = 0);
	Array_Fixed(Array_Fixed const & source);
	~Array_Fixed() = default;

	Array_Fixed & operator=(Array_Fixed const & source);
	T const & operator[](u32 i) const;
	T & operator[](u32 i);
};

}
