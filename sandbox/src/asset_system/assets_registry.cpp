#include "custom_pch.h"

#include "engine/api/internal/names_lookup.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/asset_system.h"

#include "asset_types.h"

#define ASSET_IMPL(T)\
	/* @Note: initialize compile-time structs: */\
	template struct custom::Array<T>;\
	template struct custom::RefT<T>;\
	template struct custom::Asset_RefT<T>;\
	/* @Note: initialize compile-time statics: */\
	custom::Ref_PoolT<T> custom::RefT<T>::pool;\
	u32 custom::Asset_Registry<T>::type;\

#include "../registry_impl/asset_types.h"

void init_client_asset_types(void) {
	// @Note: initialize runtime assets' data:
	#define ASSET_IMPL(T)\
		custom::Asset_Registry<T>::type = custom::asset_names.get_count();\
		custom::asset_names.store_string(#T, custom::empty_index);\

	#include "../registry_impl/asset_types.h"

	custom::Asset::vtable.asset_constructors.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.asset_destructors.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.asset_containers.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.asset_loaders.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.asset_unloaders.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.asset_updaters.set_capacity(custom::asset_names.get_count());
	#define ASSET_IMPL(T)\
		custom::Asset::vtable.asset_constructors.push(&custom::ref_pool_create<T>);\
		custom::Asset::vtable.asset_destructors.push(&custom::ref_pool_destroy<T>);\
		custom::Asset::vtable.asset_containers.push(&custom::ref_pool_contains<T>);\
		custom::Asset::vtable.asset_loaders.push(&custom::loading::asset_pool_load<T>);\
		custom::Asset::vtable.asset_unloaders.push(&custom::loading::asset_pool_unload<T>);\
		custom::Asset::vtable.asset_updaters.push(&custom::loading::asset_pool_update<T>);\

	#include "../registry_impl/asset_types.h"
}
