#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/api/internal/asset_system.h"
#include "engine/impl/array.h"


namespace custom {

//  @Note: initialize compile-time structs:
template struct Array<Asset>;

//  @Note: initialize compile-time statics:
Array<Asset>           Asset::instances;
Array<u32>             Asset::ids;
Array<u32>             Asset::types;
Strings_Storage        Asset::paths;

Array<ref_void_func *> Asset::asset_constructors;
Array<void_ref_func *> Asset::asset_destructors;
Array<bool_ref_func *> Asset::asset_containers;
Array<loading_func *>  Asset::asset_loaders;
Array<loading_func *>  Asset::asset_unloaders;

}

//
// asset
//

namespace custom {

static u32 find(u32 type, Array<u32> const & types, Array<u32> const & ids, u32 id) {
	for (u32 i = 0; i < ids.count; ++i) {
		if (types[i] != type) { continue; }
		if (ids[i] == id) { return i; }
	}
	return custom::empty_index;
}

static u32 find(u32 type, Array<u32> const & types, Array<Asset> const & instances, Asset const & ref) {
	for (u32 i = 0; i < instances.count; ++i) {
		if (types[i] != type) { continue; }
		if (instances[i] == ref) { return i; }
	}
	return custom::empty_index;
}

Asset Asset::add(u32 type, u32 id, bool or_get) {
	u32 index = find(type, Asset::types, Asset::ids, id);
	if (index == custom::empty_index) {
		index = Asset::ids.count;
		Asset::ids.push(id);
		Asset::instances.push({custom::empty_ref});
		Asset::types.push(type);
	}

	Asset & ref = Asset::instances[index];

	if (ref.id == custom::empty_ref.id || !(*Asset::asset_containers[type])(ref)) {
		ref = {(*Asset::asset_constructors[type])()};
		(*Asset::asset_loaders[type])(ref);
	}
	else { CUSTOM_ASSERT(or_get, "asset already exists"); }

	return ref;
}

void Asset::rem(u32 type, u32 id) {
	u32 index = find(type, Asset::types, Asset::ids, id);
	if (index == custom::empty_index) {
		CUSTOM_ASSERT(false, "asset doesn't exist"); return;
	}

	Ref ref = Asset::instances[index];

	Asset::ids.remove_at(index);
	Asset::instances.remove_at(index);
	Asset::types.remove_at(index);

	if ((*Asset::asset_containers[type])(ref)) {
		(*Asset::asset_unloaders[type])(ref);
		(*Asset::asset_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "asset doesn't exist"); }
}

Asset Asset::get(u32 type, u32 id) {
	u32 index = find(type, Asset::types, Asset::ids, id);
	if (index == custom::empty_index) { return {custom::empty_ref}; }

	return Asset::instances[index];
}

bool Asset::has(u32 type, u32 id) {
	u32 index = find(type, Asset::types, Asset::ids, id);
	if (index == custom::empty_index) { return false; }

	Ref const & ref = Asset::instances[index];
	return (*Asset::asset_containers[type])(ref);
}

u32 Asset::store_path(cstring data, u32 length) {
	return paths.get_or_add_id(data, length);
}

cstring Asset::get_path(u32 type, Asset const & ref) {
	u32 index = find(type, Asset::types, Asset::instances, ref);
	if (index == custom::empty_index) { return NULL; }

	return paths.get_value(Asset::ids[index]);
}

}
