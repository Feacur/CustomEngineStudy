#pragma once
#include "reference.h"
#include "strings_storage.h"

namespace custom {
	// @Forward
	struct Asset;
}

//
// universal access
//

namespace custom {

#define LOADING_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Asset & asset_ref)
typedef LOADING_FUNC(loading_func);

}

//
// asset
//

namespace custom {

template<typename T> struct Asset_RefT
{
	RefT<T> ref; u32 resource;

	cstring get_path(void) const;
	bool exists(void) const;
	void destroy(void);
};

template<typename T> struct Asset_Registry { static u32 type; };

// @Todo: separate Asset_System container from Asset? and make the system container an instance?
struct Asset : public Ref
{
	u32 resource;
	u32 type;

	// instances
	static Array<Ref> instance_refs;
	static Array<u32> resources;
	static Array<u32> types;
	static Strings_Storage strings;

	// strings API
	static u32 store_string(cstring data, u32 length);
	static u32 get_resource(cstring data, u32 length);
	static cstring get_string(u32 id);

	// types API
	static Array<ref_void_func *> asset_constructors;
	static Array<void_ref_func *> asset_destructors;
	static Array<bool_ref_func *> asset_containers;
	static Array<loading_func *>  asset_loaders;
	static Array<loading_func *>  asset_unloaders;
	static Array<loading_func *>  asset_updaters;

	static void update(void);

	static void reset_system(u32 type);
	static Asset add(u32 type, u32 resource);
	static void  rem(u32 type, u32 resource);
	static Asset get(u32 type, u32 resource);
	static bool  has(u32 type, u32 resource);

	cstring get_path(void) const;
	bool exists(void) const;
	void destroy(void);

	template<typename T> static Asset_RefT<T> add(u32 resource);
	template<typename T> static void          rem(u32 resource);
	template<typename T> static Asset_RefT<T> get(u32 resource);
	template<typename T> static bool          has(u32 resource);
};

}
