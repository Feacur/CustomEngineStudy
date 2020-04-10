#pragma once
#include "engine/core/types.h"

// @Note: this code implies POD types
//        all of the T, Array<T>, Array_Fixed<T, N>

namespace custom {

template<typename T>
struct Array
{
	T * data;
	u32 capacity, count;

	Array(u32 capacity = 0, u32 count = 0);
	#if !defined(CUSTOM_SHIPPING)
		Array(Array const & source); // @Note: warn if a copy occures
	#endif
	~Array();

	#if !defined(CUSTOM_SHIPPING)
		Array & operator=(Array const & source); // @Note: warn if a copy occures
	#endif
	T const & operator[](u32 i) const;
	T & operator[](u32 i);

	void set_capacity(u32 number);
	void ensure_capacity(u32 number);

	void push();
	void push(T const & value);
	void push_range(u32 number);
	void push_range(T const * values, u32 number);

	void insert(u32 i);
	void insert(u32 i, T const & value);

	void pop();
	void remove_at(u32 i);
	void remove_at_ordered(u32 i);
};

template<typename T, u16 capacity>
struct Array_Fixed
{
	T data[capacity];
	u16 count;

	Array_Fixed(u16 count = 0);
	#if !defined(CUSTOM_SHIPPING)
		Array_Fixed(Array_Fixed const & source); // @Note: warn if a copy occures
	#endif
	~Array_Fixed();

	#if !defined(CUSTOM_SHIPPING)
		Array_Fixed & operator=(Array_Fixed const & source); // @Note: warn if a copy occures
	#endif
	T const & operator[](u16 i) const;
	T & operator[](u16 i);

	void push();
	void push(T const & value);
	void push_range(u16 number);
	void push_range(T const * values, u16 number);

	void insert(u16 i);
	void insert(u16 i, T const & value);

	void pop();
	void remove_at(u16 i);
	void remove_at_ordered(u16 i);
};

}
