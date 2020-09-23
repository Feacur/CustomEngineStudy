#pragma once
#include "engine/core/types.h"

#define COLLECTION_COPY_WARNING

#if defined(CUSTOM_SHIPPING)
	#undef COLLECTION_COPY_WARNING
#endif

// @Note: this code implies POD types
//        all of the T, Array<T>, Array_Fixed<T, N>

namespace custom {

constexpr u32 const empty_index = UINT32_MAX;

template<typename T>
struct Array
{
	T * data;
	u32 capacity, count;

	Array(u32 capacity = 0, u32 count = 0);
	#if defined(COLLECTION_COPY_WARNING)
		Array(Array const & source);
	#endif
	~Array(void);

	#if defined(COLLECTION_COPY_WARNING)
		Array & operator=(Array const & source);
	#endif
	T const & operator[](u32 i) const;
	T & operator[](u32 i);
	T const & get(u32 i) const;
	T & get(u32 i);

	void set_capacity(u32 number);
	void ensure_capacity(u32 number);

	void push(void);
	void push(T const & value);
	void push_range(u32 number);
	void push_range(T const * values, u32 number);

	void insert(u32 i);
	void insert(u32 i, T const & value);

	void pop(void);
	void pop_range(u32 number);
	void remove_at(u32 i);
	void remove_at_ordered(u32 i);
};

template<typename T, u16 N>
struct Array_Fixed
{
	T data[N];
	constexpr static u16 const capacity = N;
	u16 count;

	Array_Fixed(u16 count = 0);
	#if defined(COLLECTION_COPY_WARNING)
		Array_Fixed(Array_Fixed const & source);
	#endif
	// ~Array_Fixed(void) = default;

	#if defined(COLLECTION_COPY_WARNING)
		Array_Fixed & operator=(Array_Fixed const & source);
	#endif
	T const & operator[](u16 i) const;
	T & operator[](u16 i);
	T const & get(u16 i) const;
	T & get(u16 i);

	void push(void);
	void push(T const & value);
	void push_range(u16 number);
	void push_range(T const * values, u16 number);

	void insert(u16 i);
	void insert(u16 i, T const & value);

	void pop(void);
	void pop_range(u16 number);
	void remove_at(u16 i);
	void remove_at_ordered(u16 i);
};

}
