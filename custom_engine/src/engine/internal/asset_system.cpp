#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/api/internal/asset_system.h"
#include "engine/impl/array.h"

namespace custom {

//  @Note: initialize compile-time statics:
Array<cstring>         Assets::paths;
Array<Ref>             Assets::instances;
Array<u32>             Assets::types;
Array<ref_void_func *> Assets::asset_constructors;
Array<void_ref_func *> Assets::asset_destructors;
Array<bool_ref_func *> Assets::asset_containers;

}

//
// assets
//

namespace custom {

static u32 find(u32 type, Array<u32> const & types, Array<cstring> const & paths, cstring id) {
	for (u32 i = 0; i < paths.count; ++i) {
		if (types[i] != type) { continue; }
		if (strcmp(paths[i], id) == 0) { return i; }
	}
	return UINT32_MAX;
}

static u32 find(u32 type, Array<u32> const & types, Array<Ref> const & instances, Ref const & ref) {
	for (u32 i = 0; i < instances.count; ++i) {
		if (types[i] != type) { continue; }
		if (instances[i] == ref) { return i; }
	}
	return UINT32_MAX;
}

Ref Assets::add(u32 type, cstring id) {
	u32 index = find(type, Assets::types, Assets::paths, id);
	if (index == UINT32_MAX) {
		index = Assets::paths.count;
		Assets::paths.push(id);
		Assets::instances.push({UINT32_MAX, 0});
		Assets::types.push(type);
	}

	Ref & ref = Assets::instances[index];

	if (ref.id == UINT32_MAX || !(*Assets::asset_containers[type])(ref)) {
		ref = (*Assets::asset_constructors[type])();
	}
	else { CUSTOM_ASSERT(false, "component already exists"); }

	return ref;
}

void Assets::rem(u32 type, cstring id) {
	u32 index = find(type, Assets::types, Assets::paths, id);
	if (index == UINT32_MAX) {
		CUSTOM_ASSERT(false, "component doesn't exist"); return;
	}

	Assets::paths.remove_at(index);
	Assets::instances.remove_at(index);
	Assets::types.remove_at(index);

	Ref const & ref = Assets::instances[index];

	if ((*Assets::asset_containers[type])(ref)) {
		(*Assets::asset_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "component doesn't exist"); }
}

Ref Assets::get(u32 type, cstring id) {
	u32 index = find(type, Assets::types, Assets::paths, id);
	if (index == UINT32_MAX) { return {UINT32_MAX, 0}; }

	return Assets::instances[index];
}

bool Assets::has(u32 type, cstring id) {
	u32 index = find(type, Assets::types, Assets::paths, id);
	if (index == UINT32_MAX) { return false; }

	Ref const & ref = Assets::instances[index];
	return (*Assets::asset_containers[type])(ref);
}

cstring Assets::get_path(u32 type, Ref const & ref) {
	u32 index = find(type, Assets::types, Assets::instances, ref);
	if (index == UINT32_MAX) { return NULL; }

	return Assets::paths[index];
}

}
