#include "custom_pch.h"
#include "engine/core/types.h"
#include "engine/core/math_types.h"
#include "engine/api/bytecode.h"
#include "engine/impl/array.h"

// @Note: thought that a separate translation unit is currently too much for this
namespace custom {

Bytecode::Bytecode()
	: buffer()
	, offset(0)
{ }

Bytecode::~Bytecode() = default;

void Bytecode::reset() {
	buffer.count = 0;
	offset = 0;
}

}

// @Note: not like I need them all now, this template definitions;
//        rather testing the idea
namespace custom {

template struct xvec2<r32>;
template struct xvec3<r32>;
template struct xvec4<r32>;

template struct xvec2<s32>;
template struct xvec3<s32>;
template struct xvec4<s32>;

template struct xvec2<u32>;
template struct xvec3<u32>;
template struct xvec4<u32>;

//
template struct Array<char>;

template struct Array<s8>;
template struct Array<s16>;
template struct Array<s32>;
template struct Array<s64>;

template struct Array<u8>;
template struct Array<u16>;
template struct Array<u32>;
template struct Array<u64>;

template struct Array<s48>;
template struct Array<u48>;

template struct Array<r32>;
template struct Array<r64>;

//
template struct Array<xvec2<r32>>;
template struct Array<xvec3<r32>>;
template struct Array<xvec4<r32>>;

template struct Array<xvec2<s32>>;
template struct Array<xvec3<s32>>;
template struct Array<xvec4<s32>>;

template struct Array<xvec2<u32>>;
template struct Array<xvec3<u32>>;
template struct Array<xvec4<u32>>;

}