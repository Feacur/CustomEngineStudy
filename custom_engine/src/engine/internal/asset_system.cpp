#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/api/internal/asset_system.h"
#include "engine/impl/array.h"

namespace custom {

//  @Note: initialize compile-time statics:
Array<cstring>         Asset_System::paths;
Array<Ref>             Asset_System::instances;
Array<u32>             Asset_System::types;
Array<ref_void_func *> Asset_System::asset_constructors;
Array<void_ref_func *> Asset_System::asset_destructors;
Array<bool_ref_func *> Asset_System::asset_containers;

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

static u32 find(u32 type, Array<u32> const & types, Array<Ref> const & instances, Ref ref) {
	for (u32 i = 0; i < instances.count; ++i) {
		if (types[i] != type) { continue; }
		if (instances[i] == ref) { return i; }
	}
	return UINT32_MAX;
}

Ref Asset_System::add_asset(u32 type, cstring id) {
	u32 index = find(type, Asset_System::types, Asset_System::paths, id);
	if (index == UINT32_MAX) {
		index = Asset_System::paths.count;
		Asset_System::paths.push(id);
		Asset_System::instances.push({UINT32_MAX, 0});
		Asset_System::types.push(type);
	}

	Ref & ref = Asset_System::instances[index];

	if (ref.id == UINT32_MAX || !(*Asset_System::asset_containers[type])(ref)) {
		ref = (*Asset_System::asset_constructors[type])();
	}
	else { CUSTOM_ASSERT(false, "component already exists"); }

	return ref;
}

void Asset_System::rem_asset(u32 type, cstring id) {
	u32 index = find(type, Asset_System::types, Asset_System::paths, id);
	if (index == UINT32_MAX) {
		CUSTOM_ASSERT(false, "component doesn't exist"); return;
	}

	Asset_System::paths.remove_at(index);
	Asset_System::instances.remove_at(index);
	Asset_System::types.remove_at(index);

	Ref const & ref = Asset_System::instances[index];

	if ((*Asset_System::asset_containers[type])(ref)) {
		(*Asset_System::asset_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "component doesn't exist"); }
}

Ref Asset_System::get_asset(u32 type, cstring id) {
	u32 index = find(type, Asset_System::types, Asset_System::paths, id);
	if (index == UINT32_MAX) { return {UINT32_MAX, 0}; }

	return Asset_System::instances[index];
}

bool Asset_System::has_asset(u32 type, cstring id) {
	u32 index = find(type, Asset_System::types, Asset_System::paths, id);
	if (index == UINT32_MAX) { return false; }

	Ref const & ref = Asset_System::instances[index];
	return (*Asset_System::asset_containers[type])(ref);
}

cstring Asset_System::get_path(u32 type, Ref ref) {
	u32 index = find(type, Asset_System::types, Asset_System::instances, ref);
	if (index == UINT32_MAX) { return NULL; }

	return Asset_System::paths[index];
}

}
