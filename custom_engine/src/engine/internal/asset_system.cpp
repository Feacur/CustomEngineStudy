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
Array<u32>             Asset::resources;
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

static u32 find(Array<u32> const & types, Array<u32> const & resources, u32 type, u32 resource) {
	for (u32 i = 0; i < resources.count; ++i) {
		if (types[i] != type) { continue; }
		if (resources[i] == resource) { return i; }
	}
	return custom::empty_index;
}

Asset Asset::add(u32 type, u32 resource) {
	u32 index = find(Asset::types, Asset::resources, type, resource);
	if (index == custom::empty_index) {
		index = Asset::resources.count;
		Asset::instance_refs.push({custom::empty_ref});
		Asset::resources.push(resource);
		Asset::types.push(type);
	}

	Asset asset = {Asset::instance_refs[index], resource, type};

	if (asset.ref.id == custom::empty_ref.id || !(*Asset::asset_containers[type])(asset.ref)) {
		asset.ref = {(*Asset::asset_constructors[type])()};
		Asset::instance_refs[index] = asset.ref;
		(*Asset::asset_loaders[type])(asset);
	}

	return asset;
}

Asset Asset::get(u32 type, u32 resource) {
	u32 index = find(Asset::types, Asset::resources, type, resource);
	if (index == custom::empty_index) { return {custom::empty_ref}; }

	return {Asset::instance_refs[index], resource, type};
}

bool Asset::has(u32 type, u32 resource) {
	u32 index = find(Asset::types, Asset::resources, type, resource);
	if (index == custom::empty_index) { return false; }

	Ref ref = Asset::instance_refs[index];
	return (*Asset::asset_containers[type])(ref);
}

cstring Asset::get_path(void) const {
	CUSTOM_ASSERT(exists(), "asset doesn't exist");

	return get_string(resource);
}

bool Asset::exists(void) const {
	return (*Asset::asset_containers[type])(ref);
}

void Asset::unload(void) {
	CUSTOM_ASSERT(exists(), "asset doesn't exist");

	(*Asset::asset_unloaders[type])(*this);
	(*Asset::asset_destructors[type])(ref);

	for (u32 i = 0; i < instance_refs.count; ++i) {
		if (instance_refs[i] != ref) { continue; }
		if (resources[i] != resource) { continue; }
		if (types[i] != type) { continue; }
		Asset::instance_refs.remove_at(i);
		Asset::resources.remove_at(i);
		Asset::types.remove_at(i);
		break;
	}
}

}
