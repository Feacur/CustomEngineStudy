#include "custom_pch.h"

#include "engine/core/math_types.h"
#include "engine/api/graphics_params.h"
#include "engine/api/internal/assets.h"
#include "engine/impl/array.h"
#include "engine/impl/bytecode.h"
#include "engine/impl/reference.h"

// @Note: thought that a separate translation unit is currently too much for this
namespace custom {

Bytecode::Bytecode(void)
	: buffer()
	, offset(0)
{ }

void Bytecode::reset(void) {
	buffer.count = 0;
	offset = 0;
}

}

// @Note: seems to have a beneficial effect on the executable size;
//        I suppose, at cost of no inlining?
namespace custom {

template struct xvec2<r32>;
template struct xvec3<r32>;
template struct xvec4<r32>;

template struct xvec2<s32>;
template struct xvec3<s32>;
template struct xvec4<s32>;

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
template struct Array<vec2>;
template struct Array<vec3>;
template struct Array<vec4>;

template struct Array<ivec2>;
template struct Array<ivec3>;
template struct Array<ivec4>;

//
#define GP_IMPL(T) template struct Array<T>;
#include "graphics_params_registry_impl.h"

}

namespace custom {

#define BYTECODE_IMPL(T)\
	template void Bytecode::write<T>(T const * data, u32 count);\
	template void Bytecode::write<T>(T const & datum);\
	template void Bytecode::write_sized_array<T>(T const * data, u32 count);\
	template void Bytecode::write_sized_array<T>(Array<T> const & data);\
	template T const * Bytecode::read<T>(u32 count) const;\
	template void Bytecode::copy<T>(T * out, u32 count) const;\

BYTECODE_IMPL(char);

BYTECODE_IMPL(s8);
BYTECODE_IMPL(s16);
BYTECODE_IMPL(s32);
BYTECODE_IMPL(s64);

BYTECODE_IMPL(u8);
BYTECODE_IMPL(u16);
BYTECODE_IMPL(u32);
BYTECODE_IMPL(u64);

BYTECODE_IMPL(s48);
BYTECODE_IMPL(u48);

BYTECODE_IMPL(r32);
BYTECODE_IMPL(r64);

BYTECODE_IMPL(vec2);
BYTECODE_IMPL(vec3);
BYTECODE_IMPL(vec4);

BYTECODE_IMPL(ivec2);
BYTECODE_IMPL(ivec3);
BYTECODE_IMPL(ivec4);

BYTECODE_IMPL(mat3);
BYTECODE_IMPL(mat4);

#define GP_IMPL(T) BYTECODE_IMPL(T)
#include "graphics_params_registry_impl.h"
#undef BYTECODE_IMPL

}

namespace custom {

#define ASSET_IMPL(T)\
	/* @Note: initialize compile-time structs: */\
	template struct custom::Array<T>;\
	template struct custom::RefT<T>;\
	/* @Note: initialize compile-time statics: */\
	custom::Ref_Pool<T> custom::RefT<T>::pool;\

ASSET_IMPL(ShaderAsset);
ASSET_IMPL(TextureAsset);
ASSET_IMPL(MeshAsset);
#undef ASSET_IMPL

}
