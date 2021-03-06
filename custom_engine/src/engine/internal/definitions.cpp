#include "custom_pch.h"

#include "engine/core/math_types.h"
#include "engine/api/graphics_params.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/bytecode.h"
#include "engine/impl/reference.h"

typedef custom::graphics::unit_id unit_id;

// @Note: initialize compile-time structs (xvec<T>)

template struct xvec2<r32>;
template struct xvec3<r32>;
template struct xvec4<r32>;

template struct xvec2<s32>;
template struct xvec3<s32>;
template struct xvec4<s32>;

template struct xvec2<u32>;
template struct xvec3<u32>;
template struct xvec4<u32>;

// @Note: initialize compile-time structs (Array<T>)
namespace custom {

template struct Array<char>;
template struct Array<cstring>;

#define DATA_TYPE_IMPL(T) template struct Array<T>;
#include "engine/registry_impl/data_type.h"

#define GRAPHICS_PARAM_IMPL(T) template struct Array<graphics::T>;
#include "engine/registry_impl/graphics_params.h"

#define COMPONENT_IMPL(T) template struct Array<T>;
#include "engine/registry_impl/component_types.h"

#define ASSET_IMPL(T) template struct Array<T>;
#include "engine/registry_impl/asset_types.h"

template struct Array<Ref>;

template struct Array<ref_void_func *>;
template struct Array<void_ref_func *>;
template struct Array<bool_ref_func *>;

}

// @Note: initialize compile-time structs (Bytecode)
namespace custom {

#define BYTECODE_IMPL(T)                                     \
template void Bytecode::write<T>(T const * data, u32 count); \
template T const * Bytecode::read<T>(u32 count) const;       \
template void Bytecode::copy<T>(T * out, u32 count) const;   \

BYTECODE_IMPL(char)
BYTECODE_IMPL(cstring)

#define DATA_TYPE_IMPL(T) BYTECODE_IMPL(T)
#include "engine/registry_impl/data_type.h"

#define GRAPHICS_PARAM_IMPL(T) BYTECODE_IMPL(graphics::T)
#include "engine/registry_impl/graphics_params.h"

#undef BYTECODE_IMPL

}

namespace custom {
namespace graphics {

u16 get_type_size(Data_Type value) {
	switch (value) {
		#define DATA_TYPE_IMPL(T) case Data_Type::T: return sizeof(T);
		#include "engine/registry_impl/data_type.h"
	}
	CUSTOM_ASSERT(false, "unknown data type %d", (u32)value);
	return 0;
}

}}