#pragma once
#include "reference.h"
#include "strings_storage.h"

//
// asset
//

namespace custom {

template<typename T> struct Asset_Registry { static u32 type; };

struct Asset : Ref
{
	u32 type;

	// instances
	static Array<Ref> instance_refs;
	static Array<u32> ids;
	static Array<u32> types;
	static Strings_Storage strings;

	// strings API
	static u32 store_string(cstring data, u32 length);
	static cstring get_string(u32 id);

	// types API
	static Array<ref_void_func *> asset_constructors;
	static Array<void_ref_func *> asset_destructors;
	static Array<bool_ref_func *> asset_containers;
	static Array<loading_func *>  asset_loaders;
	static Array<loading_func *>  asset_unloaders;

	static Ref  add(u32 type, u32 id);
	static void rem(u32 type, u32 id);
	static Ref  get(u32 type, u32 id);
	static bool has(u32 type, u32 id);

	static cstring get_path(Asset const & asset);

	template<typename T> static RefT<T> add(u32 id);
	template<typename T> static void    rem(u32 id);
	template<typename T> static RefT<T> get(u32 id);
	template<typename T> static bool    has(u32 id);

	template<typename T> static cstring get_path(RefT<T> const & ref);
};

}
