#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/asset_system.h"
#include "engine/impl/array.h"

namespace custom {

//  @Note: initialize compile-time structs:
template struct Array<Asset>;

//  @Note: initialize compile-time statics:
Array<Ref>             Asset::instance_refs;
Array<u32>             Asset::ids;
Array<u32>             Asset::types;
Strings_Storage        Asset::strings;

Array<ref_void_func *> Asset::asset_constructors;
Array<void_ref_func *> Asset::asset_destructors;
Array<bool_ref_func *> Asset::asset_containers;
Array<loading_func *>  Asset::asset_loaders;
Array<loading_func *>  Asset::asset_unloaders;

}

//
// strings API
//

namespace custom {

u32 Asset::store_string(cstring data, u32 length) {
	return strings.store_string(data, length);
}

cstring Asset::get_string(u32 id) {
	return strings.get_string(id);
}

}

//
// asset
//

namespace custom {

static u32 find(Array<u32> const & types, Array<u32> const & ids, u32 type, u32 id) {
	for (u32 i = 0; i < ids.count; ++i) {
		if (types[i] != type) { continue; }
		if (ids[i] == id) { return i; }
	}
	return custom::empty_index;
}

static u32 find(Array<u32> const & types, Array<Ref> const & instance_refs, Asset const & asset) {
	for (u32 i = 0; i < instance_refs.count; ++i) {
		if (types[i] != asset.type) { continue; }
		if (instance_refs[i] == asset) { return i; }
	}
	return custom::empty_index;
}

Ref Asset::add(u32 type, u32 id) {
	u32 index = find(Asset::types, Asset::ids, type, id);
	if (index == custom::empty_index) {
		index = Asset::ids.count;
		Asset::ids.push(id);
		Asset::instance_refs.push({custom::empty_ref});
		Asset::types.push(type);
	}

	Ref ref = Asset::instance_refs[index];

	if (ref.id == custom::empty_ref.id || !(*Asset::asset_containers[type])(ref)) {
		ref = {(*Asset::asset_constructors[type])()};
		Asset::instance_refs[index] = ref;
		(*Asset::asset_loaders[type])(ref);
	}

	return ref;
}

void Asset::rem(u32 type, u32 id) {
	u32 index = find(Asset::types, Asset::ids, type, id);
	if (index == custom::empty_index) {
		CUSTOM_ASSERT(false, "asset doesn't exist"); return;
	}

	Ref ref = Asset::instance_refs[index];

	Asset::ids.remove_at(index);
	Asset::instance_refs.remove_at(index);
	Asset::types.remove_at(index);

	if ((*Asset::asset_containers[type])(ref)) {
		(*Asset::asset_unloaders[type])(ref);
		(*Asset::asset_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "asset doesn't exist"); }
}

Ref Asset::get(u32 type, u32 id) {
	u32 index = find(Asset::types, Asset::ids, type, id);
	if (index == custom::empty_index) { return {custom::empty_ref}; }

	return Asset::instance_refs[index];
}

bool Asset::has(u32 type, u32 id) {
	u32 index = find(Asset::types, Asset::ids, type, id);
	if (index == custom::empty_index) { return false; }

	Ref ref = Asset::instance_refs[index];
	return (*Asset::asset_containers[type])(ref);
}

cstring Asset::get_path(Asset const & asset) {
	u32 index = find(Asset::types, Asset::instance_refs, asset);
	CUSTOM_ASSERT(index != custom::empty_index, "asset doesn't exist");
	return get_string(Asset::ids[index]);
}

}
