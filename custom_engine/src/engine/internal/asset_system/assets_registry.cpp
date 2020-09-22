#include "custom_pch.h"

#include "engine/api/internal/names_lookup.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/asset_system.h"

#define ASSET_IMPL(T) typedef custom::T T;
#include "engine/registry_impl/asset_types.h"

#define ASSET_IMPL(T)                         \
template struct custom::Array<T>;             \
template struct custom::RefT<T>;              \
template struct custom::Asset_RefT<T>;        \

#include "engine/registry_impl/asset_types.h"

namespace custom {

#define ASSET_IMPL(T)                                                               \
static REF_VOID_FUNC(ref_pool_create_##T) { return RefT<T>::pool.create(); }        \
static VOID_REF_FUNC(ref_pool_destroy_##T) { RefT<T>::pool.destroy(ref); }          \
static BOOL_REF_FUNC(ref_pool_contains_##T) { return RefT<T>::pool.contains(ref); } \

#include "engine/registry_impl/asset_types.h"

}

#include "loading.cpp.inl"
// namespace custom {
// namespace loading {
// 
// #define ASSET_IMPL(T)                \
// LOADING_FUNC(asset_pool_load_##T);   \
// LOADING_FUNC(asset_pool_unload_##T); \
// LOADING_FUNC(asset_pool_update_##T); \
// 
// #include "engine/registry_impl/asset_types.h"
// 
// }}

void init_asset_types(void) {
	#define ASSET_IMPL(T)                                              \
	custom::Asset_Registry<T>::type = custom::asset_names.get_count(); \
	custom::asset_names.store_string(#T, custom::empty_index);         \

	#include "engine/registry_impl/asset_types.h"

	custom::Asset::vtable.create.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.destroy.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.contains.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.load.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.unload.set_capacity(custom::asset_names.get_count());
	custom::Asset::vtable.update.set_capacity(custom::asset_names.get_count());

	#define ASSET_IMPL(T)                                                       \
	custom::Asset::vtable.create.push(&custom::ref_pool_create_##T);            \
	custom::Asset::vtable.destroy.push(&custom::ref_pool_destroy_##T);          \
	custom::Asset::vtable.contains.push(&custom::ref_pool_contains_##T);        \
	custom::Asset::vtable.load.push(&custom::loading::asset_pool_load_##T);     \
	custom::Asset::vtable.unload.push(&custom::loading::asset_pool_unload_##T); \
	custom::Asset::vtable.update.push(&custom::loading::asset_pool_update_##T); \

	#include "engine/registry_impl/asset_types.h"
}
