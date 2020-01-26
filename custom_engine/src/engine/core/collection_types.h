#pragma once
#include "engine/core/types.h"

#define CUSTOM_ARRAY_POD
#define CUSTOM_ARRAY_WARN

namespace custom {

template<typename T>
struct Array
{
	T * data;
	u32 capacity, count;

	Array(u32 capacity = 0, u32 count = 0);
	#if !defined(CUSTOM_ARRAY_POD) || defined(CUSTOM_ARRAY_WARN) // constructor
	Array(Array const & source);
	Array(Array && source);
	#endif // !defined(CUSTOM_ARRAY_POD) || defined(CUSTOM_ARRAY_WARN) // constructor
	~Array();

	#if !defined(CUSTOM_ARRAY_POD) || defined(CUSTOM_ARRAY_WARN) // operator=
	Array & operator=(Array const & source);
	Array & operator=(Array && source);
	#endif // !defined(CUSTOM_ARRAY_POD) || defined(CUSTOM_ARRAY_WARN) // operator=
	T const & operator[](u32 i) const;
	T & operator[](u32 i);

	void set_capacity(u32 amount);
	void ensure_capacity(u32 amount);

	void push();
	void push(T const & value);
	// void push_move(T && value);
	void push_range(u32 amount);
	void push_range(T const * values, u32 amount);
	void insert(u32 i);
	void insert(u32 i, T const & value);
	// void insert_move(u32 i, T && value);

	void pop();
	void remove(u32 i);
	void remove_ordered(u32 i);
};

template<typename T, u16 capacity>
struct Array_Fixed
{
	T data[capacity];
	u16 count;

	Array_Fixed(u16 count = 0);
	Array_Fixed(Array_Fixed const & source);
	~Array_Fixed();

	Array_Fixed & operator=(Array_Fixed const & source);
	T const & operator[](u16 i) const;
	T & operator[](u16 i);

	void push();
	void push(T const & value);
	void push_range(u16 amount);
	void push_range(T const * values, u16 amount);
	void insert(u16 i);
	void insert(u16 i, T const & value);

	void pop();
	void remove(u16 i);
	void remove_ordered(u16 i);
};

}
