#pragma once
#include "engine/core/types.h"

namespace custom {

template<typename T>
struct Array
{
	T * data;
	u32 capacity, count;

	Array(u32 capacity = 0, u32 count = 0);
	Array(Array const & source) = delete;
	~Array();

	Array & operator=(Array const & source) = delete;
	T const & operator[](u32 i) const;
	T & operator[](u32 i);

	void resize(u32 amount);

	void add();
	void add(T const & value);

	void insert(u32 i);
	void insert(u32 i, T const & value);

	void remove(u32 i);
	void remove_ordered(u32 i);
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
	T const & operator[](u16 i) const;
	T & operator[](u16 i);

	void add();
	void add(T const & value);

	void insert(u16 i);
	void insert(u16 i, T const & value);

	void remove(u16 i);
	void remove_ordered(u16 i);
};

}
