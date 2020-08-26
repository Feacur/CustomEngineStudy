#pragma once
#include "reference.h"

//
// asset
//

namespace custom {

template<typename T> struct Asset_Registry { static u32 type; };

struct Asset : Ref
{
	// instances
	static Array<Asset> instances;
	static Array<u32>   paths_todo_strings_index;
	static Array<u32>   types;

	// types API
	static Array<ref_void_func *> asset_constructors;
	static Array<void_ref_func *> asset_destructors;
	static Array<bool_ref_func *> asset_containers;
	static Array<loading_func *>  asset_loaders;
	static Array<loading_func *>  asset_unloaders;

	static Asset add(u32 type, u32 id, bool or_get);
	static void  rem(u32 type, u32 id);
	static Asset get(u32 type, u32 id);
	static bool  has(u32 type, u32 id);

	static cstring get_path(u32 type, Asset const & ref);

	template<typename T> static RefT<T> add(u32 id, bool or_get);
	template<typename T> static void    rem(u32 id);
	template<typename T> static RefT<T> get(u32 id);
	template<typename T> static bool    has(u32 id);

	template<typename T> static cstring get_path(RefT<T> const & ref);
};

}
