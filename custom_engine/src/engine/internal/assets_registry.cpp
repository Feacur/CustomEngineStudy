#include "custom_pch.h"

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

void init_asset_system(void) {
	// @Note: initialize runtime assets' data:
	u32 asset_types_count = 0;
	#define ASSET_IMPL(T) custom::Asset_Registry<T>::type = asset_types_count++;
	#include "assets_registry_impl.h"

	custom::Asset::asset_constructors.set_capacity(asset_types_count);
	custom::Asset::asset_destructors.set_capacity(asset_types_count);
	custom::Asset::asset_containers.set_capacity(asset_types_count);
	#define ASSET_IMPL(T)\
		custom::Asset::asset_constructors.push(&custom::ref_pool_create<T>);\
		custom::Asset::asset_destructors.push(&custom::ref_pool_destroy<T>);\
		custom::Asset::asset_containers.push(&custom::ref_pool_contains<T>);\

	#include "assets_registry_impl.h"
}

namespace custom {
namespace asset {

#define ASSET_IMPL(T) #T,
cstring names[] = {
	#include "assets_registry_impl.h"
	NULL
};

u32 count = C_ARRAY_LENGTH(names) - 1;

}}
