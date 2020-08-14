#include "custom_pch.h"

#include "engine/api/internal/assets.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"

// @Note: initialize compile-time structs (Asset)
#define ASSET_IMPL(T)\
	/* @Note: initialize compile-time structs: */\
	template struct custom::Array<T>;\
	template struct custom::RefT<T>;\
	/* @Note: initialize compile-time statics: */\
	custom::Ref_Pool<T> custom::RefT<T>::pool;\

ASSET_IMPL(custom::ShaderAsset);
ASSET_IMPL(custom::TextureAsset);
ASSET_IMPL(custom::MeshAsset);
#undef ASSET_IMPL
