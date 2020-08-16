#include "custom_pch.h"

#include "engine/api/internal/types_names_lookup.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/asset_system.h"

#define ASSET_IMPL(T) typedef custom::T T;
#include "assets_registry_impl.h"

#define ASSET_IMPL(T)\
	/* @Note: initialize compile-time structs: */\
	template struct custom::Array<T>;\
	template struct custom::RefT<T>;\
	/* @Note: initialize compile-time statics: */\
	custom::Ref_Pool<T> custom::RefT<T>::pool;\
	u32 custom::Asset_Registry<T>::type;\

#include "assets_registry_impl.h"

void init_asset_types(void) {
	// @Note: initialize runtime assets' data:
	#define ASSET_IMPL(T)\
		custom::Asset_Registry<T>::type = custom::asset_names.count;\
		custom::asset_names.push(#T);\

	#include "assets_registry_impl.h"

	custom::Asset::asset_constructors.set_capacity(custom::asset_names.count);
	custom::Asset::asset_destructors.set_capacity(custom::asset_names.count);
	custom::Asset::asset_containers.set_capacity(custom::asset_names.count);
	custom::Asset::asset_loaders.set_capacity(custom::asset_names.count);
	custom::Asset::asset_unloaders.set_capacity(custom::asset_names.count);
	#define ASSET_IMPL(T)\
		custom::Asset::asset_constructors.push(&custom::ref_pool_create<T>);\
		custom::Asset::asset_destructors.push(&custom::ref_pool_destroy<T>);\
		custom::Asset::asset_containers.push(&custom::ref_pool_contains<T>);\
		custom::Asset::asset_loaders.push(&custom::asset_pool_load<T>);\
		custom::Asset::asset_unloaders.push(&custom::asset_pool_unload<T>);\

	#include "assets_registry_impl.h"
}
