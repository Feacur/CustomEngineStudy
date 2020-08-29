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
// asset ref
//

namespace custom {

inline static u32 is_correct(Asset const & asset) {
	for (u32 i = 0; i < Asset::types.count; ++i) {
		if (Asset::types[i] != asset.type) { continue; }
		if (Asset::resources[i] != asset.resource) { continue; }
		if (Asset::instance_refs[i] == asset.ref) { return true; }
	}
	return false;
}

cstring Asset::get_path(void) const {
	CUSTOM_ASSERT(is_correct(*this), "asset ref is corrupted");
	return get_string(resource);
}

bool Asset::exists(void) const {
	CUSTOM_ASSERT(is_correct(*this), "asset ref is corrupted");
	return (*Asset::asset_containers[type])(ref);
}

void Asset::destroy(void) {
	// @Note: duplicates `Asset::rem` code
	CUSTOM_ASSERT(is_correct(*this), "asset ref is corrupted");
	if ((*Asset::asset_containers[type])(ref)) {
		(*Asset::asset_unloaders[type])(*this);
		(*Asset::asset_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "asset doesn't exist"); }

	for (u32 i = 0; i < instance_refs.count; ++i) {
		if (types[i] != type) { continue; }
		if (resources[i] != resource) { continue; }
		if (instance_refs[i] != ref) { continue; }
		Asset::instance_refs.remove_at(i);
		Asset::resources.remove_at(i);
		Asset::types.remove_at(i);
		break;
	}
}

}

//
// asset
//

namespace custom {

static u32 find(u32 type, u32 resource) {
	for (u32 i = 0; i < Asset::types.count; ++i) {
		if (Asset::types[i] != type) { continue; }
		if (Asset::resources[i] == resource) { return i; }
	}
	return custom::empty_index;
}

Asset Asset::add(u32 type, u32 resource) {
	Asset asset = {custom::empty_ref, resource, type};

	u32 index = find(type, resource);
	if (index != custom::empty_index) { asset.ref = Asset::instance_refs[index]; }

	if (asset.ref.id == custom::empty_ref.id || !(*Asset::asset_containers[type])(asset.ref)) {
		if (index != custom::empty_index) {
			Asset::instance_refs.remove_at(index);
			Asset::resources.remove_at(index);
			Asset::types.remove_at(index);
		}

		asset.ref = {(*Asset::asset_constructors[type])()};
		Asset::instance_refs.push(asset.ref);
		Asset::resources.push(resource);
		Asset::types.push(type);

		(*Asset::asset_loaders[type])(asset);
	}
	// @Todo: check explicitly?
	//else { CUSTOM_ASSERT(false, "asset already exists"); }

	return asset;
}

void Asset::rem(u32 type, u32 resource) {
	// @Note: duplicates `Asset::destroy` code
	Asset asset = {custom::empty_ref, resource, type};

	u32 index = find(type, resource);
	if (index != custom::empty_index) {
		asset.ref = Asset::instance_refs[index];
		Asset::instance_refs.remove_at(index);
		Asset::resources.remove_at(index);
		Asset::types.remove_at(index);
	}

	if ((*Asset::asset_containers[type])(asset.ref)) {
		(*Asset::asset_unloaders[type])(asset);
		(*Asset::asset_destructors[type])(asset.ref);
	}
	else { CUSTOM_ASSERT(false, "asset doesn't exist"); }
}

Asset Asset::get(u32 type, u32 resource) {
	u32 index = find(type, resource);
	if (index == custom::empty_index) { return {custom::empty_ref}; }

	return {Asset::instance_refs[index], resource, type};
}

bool Asset::has(u32 type, u32 resource) {
	u32 index = find(type, resource);
	if (index == custom::empty_index) { return false; }

	Ref ref = Asset::instance_refs[index];
	return (*Asset::asset_containers[type])(ref);
}

}
