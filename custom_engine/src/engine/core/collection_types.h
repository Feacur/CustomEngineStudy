#pragma once
#include "engine/core/types.h"

namespace custom {

template<typename T>
struct Array
{
	T * data;
	u32 capacity, count;

	//
	// language facilities
	//
	Array(u16 capacity = 0, u16 count = 0);
	~Array();

	T const & operator[](u32 i) const;
	T & operator[](u32 i);

	//
	// should not be copied (or should it be?)
	//
	Array(Array const & source) = delete;
	Array & operator=(Array const & source) = delete;

	//
	// API
	//
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

	//
	// language facilities
	//
	Array_Fixed(u16 count = 0);
	Array_Fixed(Array_Fixed const & source);
	~Array_Fixed() = default;

	Array_Fixed & operator=(Array_Fixed const & source);
	T const & operator[](u32 i) const;
	T & operator[](u32 i);

	//
	// API
	//
};

}
