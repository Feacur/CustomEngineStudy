#pragma once
#include "reference.h"

//
// assets
//

namespace custom {

template<typename T> struct Asset_Registry { static u32 type; };

struct Assets
{
	// instances
	static Array<cstring> paths;
	static Array<Ref> instances;
	static Array<u32> types;

	// types API
	static Array<ref_void_func *> asset_constructors;
	static Array<void_ref_func *> asset_destructors;
	static Array<bool_ref_func *> asset_containers;

	static Ref  add(u32 type, cstring id);
	static void rem(u32 type, cstring id);
	static Ref  get(u32 type, cstring id);
	static bool has(u32 type, cstring id);

	static cstring get_path(u32 type, Ref const & ref);

	template<typename T> static RefT<T> add(cstring id);
	template<typename T> static void    rem(cstring id);
	template<typename T> static RefT<T> get(cstring id);
	template<typename T> static bool    has(cstring id);

	template<typename T> static cstring get_path(RefT<T> const & ref);
};

}
