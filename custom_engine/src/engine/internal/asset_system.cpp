#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/api/internal/asset_system.h"
#include "engine/impl/array.h"

namespace custom {

//  @Note: initialize compile-time structs:
template struct Array<Asset>;

//  @Note: initialize compile-time statics:
Array<Asset>           Asset::instances;
Array<cstring>         Asset::paths;
Array<u32>             Asset::types;
Array<ref_void_func *> Asset::asset_constructors;
Array<void_ref_func *> Asset::asset_destructors;
Array<bool_ref_func *> Asset::asset_containers;
Array<void_dref_func *> Asset::asset_loaders;
Array<void_dref_func *> Asset::asset_unloaders;

}

//
// asset
//

namespace custom {

static u32 find(u32 type, Array<u32> const & types, Array<cstring> const & paths, cstring id) {
	for (u32 i = 0; i < paths.count; ++i) {
		if (types[i] != type) { continue; }
		if (strcmp(paths[i], id) == 0) { return i; }
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

Asset Asset::add(u32 type, cstring id, bool or_get) {
	u32 index = find(type, Asset::types, Asset::paths, id);
	if (index == custom::empty_index) {
		index = Asset::paths.count;
		Asset::paths.push(id);
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

void Asset::rem(u32 type, cstring id) {
	u32 index = find(type, Asset::types, Asset::paths, id);
	if (index == custom::empty_index) {
		CUSTOM_ASSERT(false, "asset doesn't exist"); return;
	}

	Ref ref = Asset::instances[index];

	Asset::paths.remove_at(index);
	Asset::instances.remove_at(index);
	Asset::types.remove_at(index);

	if ((*Asset::asset_containers[type])(ref)) {
		(*Asset::asset_unloaders[type])(ref);
		(*Asset::asset_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "asset doesn't exist"); }
}

Asset Asset::get(u32 type, cstring id) {
	u32 index = find(type, Asset::types, Asset::paths, id);
	if (index == custom::empty_index) { return {custom::empty_ref}; }

	return Asset::instances[index];
}

bool Asset::has(u32 type, cstring id) {
	u32 index = find(type, Asset::types, Asset::paths, id);
	if (index == custom::empty_index) { return false; }

	Ref const & ref = Asset::instances[index];
	return (*Asset::asset_containers[type])(ref);
}

cstring Asset::get_path(u32 type, Asset const & ref) {
	u32 index = find(type, Asset::types, Asset::instances, ref);
	if (index == custom::empty_index) { return NULL; }

	return Asset::paths[index];
}

}
