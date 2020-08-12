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

// @Note: initialize compile-time structs (xvec<T>)
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

}

// @Note: initialize compile-time structs (Array<T>)
namespace custom {

template struct Array<char>;

#define DATA_TYPE_IMPL(T) template struct Array<T>;
#include "engine/api/data_type_registry_impl.h"

#define GP_IMPL(T) template struct Array<T>;
#include "graphics_params_registry_impl.h"

}

// @Note: initialize compile-time structs (Bytecode)
namespace custom {

#define BYTECODE_IMPL(T)\
	template void Bytecode::write<T>(T const * data, u32 count);\
	template void Bytecode::write<T>(T const & datum);\
	template void Bytecode::write_sized_array<T>(T const * data, u32 count);\
	template void Bytecode::write_sized_array<T>(Array<T> const & data);\
	template T const * Bytecode::read<T>(u32 count) const;\
	template void Bytecode::copy<T>(T * out, u32 count) const;\

BYTECODE_IMPL(char)

#define DATA_TYPE_IMPL(T) BYTECODE_IMPL(T)
#include "engine/api/data_type_registry_impl.h"

#define GP_IMPL(T) BYTECODE_IMPL(T)
#include "graphics_params_registry_impl.h"

#undef BYTECODE_IMPL

}

// @Note: initialize compile-time structs (Asset)
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
