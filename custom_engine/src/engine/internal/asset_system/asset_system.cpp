#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/names_lookup.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/platform/file.h"
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
Array<loading_func *>  Asset::asset_updaters;

}

//
// strings API
//

namespace custom {

u32 Asset::store_string(cstring data, u32 length) {
	return strings.store_string(data, length);
}

u32 Asset::get_resource(cstring data, u32 length) {
	return strings.get_id(data, length);
}

cstring Asset::get_string(u32 id) {
	return strings.get_string(id);
}

}

//
//
//

namespace custom {

static u32 find_by_resource(u32 resource) {
	for (u32 i = 0; i < Asset::types.count; ++i) {
		if (Asset::resources[i] == resource) { return i; }
	}
	return custom::empty_index;
}

void Asset::update(void) {
	typedef custom::file::Action_Type Action_Type;

	custom::file::watch_update();
	for (u32 i = 0; i < file::actions.count; ++i) {
		custom::file::Action const & action = custom::file::actions[i];
		cstring string = custom::file::strings.get_string(action.id);
		u32 length = custom::file::strings.get_length(action.id);
		switch (action.type) {
			case Action_Type::Add: CUSTOM_TRACE("@Todo: add '%s'", string); break;
			case Action_Type::Rem: CUSTOM_TRACE("@Todo: rem '%s'", string); break;

			case Action_Type::Mod: {
				u32 resource = strings.get_id(string, length);
				u32 index = find_by_resource(resource);
				if (index != custom::empty_index) {
					Asset asset = {instance_refs[index], resources[index], types[index]};
					(*Asset::asset_updaters[asset.type])(asset);
				}
			} break;

			case Action_Type::Old: CUSTOM_TRACE("@Todo: old '%s'", string); break;
			case Action_Type::New: CUSTOM_TRACE("@Todo: new '%s'", string); break;
		}
	}
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
		if (Asset::instance_refs[i] == asset) { return true; }
	}
	return false;
}

cstring Asset::get_path(void) const {
	CUSTOM_ASSERT(is_correct(*this), "asset ref is corrupted");
	return get_string(resource);
}

bool Asset::exists(void) const {
	CUSTOM_ASSERT(is_correct(*this), "asset ref is corrupted");
	return (*Asset::asset_containers[type])(*this);
}

void Asset::destroy(void) {
	// @Note: duplicates `Asset::rem` code
	CUSTOM_ASSERT(is_correct(*this), "asset ref is corrupted");
	if ((*Asset::asset_containers[type])(*this)) {
		(*Asset::asset_unloaders[type])(*this);
		(*Asset::asset_destructors[type])(*this);
	}
	else { CUSTOM_ASSERT(false, "asset doesn't exist"); }

	for (u32 i = 0; i < instance_refs.count; ++i) {
		if (types[i] != type) { continue; }
		if (resources[i] != resource) { continue; }
		if (instance_refs[i] != *this) { continue; }
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

void Asset::reset_system(u32 type) {
	Array<Asset> instances_of_type;
	for (u32 i = 0; i < instance_refs.count; ++i) {
		if (types[i] == type) {
			instances_of_type.push({instance_refs[i], resources[i], types[i]});
		}
	}

	for (u32 i = 0; i < instances_of_type.count; ++i) {
		instances_of_type[i].destroy();
	}

	for (u32 i = 0; i < instance_refs.count; ++i) {
		if (types[i] != type) { continue; }
		CUSTOM_ASSERT(false, "still some assets");
	}
}

Asset Asset::add(u32 type, u32 resource) {
	Asset asset = {custom::empty_ref, resource, type};

	u32 index = find(type, resource);
	if (index != custom::empty_index) {
		Ref asset_ref = Asset::instance_refs[index];
		asset.id  = asset_ref.id;
		asset.gen = asset_ref.gen;
	}

	if (asset.id == custom::empty_ref.id || !(*Asset::asset_containers[type])(asset)) {
		if (index != custom::empty_index) {
			Asset::instance_refs.remove_at(index);
			Asset::resources.remove_at(index);
			Asset::types.remove_at(index);
		}

		Ref asset_ref = (*Asset::asset_constructors[type])();
		asset.id  = asset_ref.id;
		asset.gen = asset_ref.gen;
		Asset::instance_refs.push(asset);
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
		Ref asset_ref = Asset::instance_refs[index];
		asset.id  = asset_ref.id;
		asset.gen = asset_ref.gen;
		Asset::instance_refs.remove_at(index);
		Asset::resources.remove_at(index);
		Asset::types.remove_at(index);
	}

	if ((*Asset::asset_containers[type])(asset)) {
		(*Asset::asset_unloaders[type])(asset);
		(*Asset::asset_destructors[type])(asset);
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
