#pragma once
#include "engine/core/types.h"

// c++ meta and operators for enum structs
#if defined(__cplusplus)
namespace meta {
	template<size_t S> struct uint_for_size;
	template<> struct uint_for_size<1> { typedef u8  type; };
	template<> struct uint_for_size<2> { typedef u16 type; };
	template<> struct uint_for_size<4> { typedef u32 type; };
	template<> struct uint_for_size<8> { typedef u64 type; };

	template<typename T>
	struct uint_for_type {
		typedef typename uint_for_size<sizeof(T)>::type type;
	};
}

#define ENUM_FLAG_OPERATORS_IMPL(T)                                                            \
constexpr inline T operator&(T a, T b) {                                                       \
    using U = meta::uint_for_type<T>::type;                                                    \
    return static_cast<T>(static_cast<U>(a) & static_cast<U>(b));                              \
}                                                                                              \
constexpr inline T operator|(T a, T b) {                                                       \
    using U = meta::uint_for_type<T>::type;                                                    \
    return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));                              \
}                                                                                              \
constexpr inline T operator^(T a, T b) {                                                       \
    using U = meta::uint_for_type<T>::type;                                                    \
    return static_cast<T>(static_cast<U>(a) ^ static_cast<U>(b));                              \
}                                                                                              \
constexpr inline T operator~(T v) {                                                            \
    using U = meta::uint_for_type<T>::type;                                                    \
    return static_cast<T>(~static_cast<U>(v));                                                 \
}                                                                                              \
constexpr inline bool bits_are_set(T container, T bits) { return (container & bits) == bits; } \
constexpr inline T bits_to_zero(T container, T bits) { return container & ~bits; }             \

#endif // defined(__cplusplus)
