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
	return UINT32_MAX;
}

static u32 find(u32 type, Array<u32> const & types, Array<Asset> const & instances, Asset const & ref) {
	for (u32 i = 0; i < instances.count; ++i) {
		if (types[i] != type) { continue; }
		if (instances[i] == ref) { return i; }
	}
	return UINT32_MAX;
}

Ref Asset::add(u32 type, cstring id) {
	u32 index = find(type, Asset::types, Asset::paths, id);
	if (index == UINT32_MAX) {
		index = Asset::paths.count;
		Asset::paths.push(id);
		Asset::instances.push({UINT32_MAX, 0});
		Asset::types.push(type);
	}

	Ref & ref = Asset::instances[index];

	if (ref.id == UINT32_MAX || !(*Asset::asset_containers[type])(ref)) {
		ref = (*Asset::asset_constructors[type])();
	}
	else { CUSTOM_ASSERT(false, "asset already exists"); }

	return ref;
}

void Asset::rem(u32 type, cstring id) {
	u32 index = find(type, Asset::types, Asset::paths, id);
	if (index == UINT32_MAX) {
		CUSTOM_ASSERT(false, "asset doesn't exist"); return;
	}

	Asset::paths.remove_at(index);
	Asset::instances.remove_at(index);
	Asset::types.remove_at(index);

	Ref const & ref = Asset::instances[index];

	if ((*Asset::asset_containers[type])(ref)) {
		(*Asset::asset_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "asset doesn't exist"); }
}

Ref Asset::get(u32 type, cstring id) {
	u32 index = find(type, Asset::types, Asset::paths, id);
	if (index == UINT32_MAX) { return {UINT32_MAX, 0}; }

	return Asset::instances[index];
}

bool Asset::has(u32 type, cstring id) {
	u32 index = find(type, Asset::types, Asset::paths, id);
	if (index == UINT32_MAX) { return false; }

	Ref const & ref = Asset::instances[index];
	return (*Asset::asset_containers[type])(ref);
}

cstring Asset::get_path(u32 type, Asset const & ref) {
	u32 index = find(type, Asset::types, Asset::instances, ref);
	if (index == UINT32_MAX) { return NULL; }

	return Asset::paths[index];
}

}
