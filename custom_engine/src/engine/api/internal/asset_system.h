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
template<typename T> u32 Asset_Registry<T>::type;

// @Todo: separate Asset_System container from Asset? and make the system container an instance?
struct Asset : public Ref
{
	u32 resource;
	u32 type;

	struct State {
		Array<Ref> instance_refs;
		Array<u32> resources;
		Array<u32> types;
	};
	struct VTable {
		Array<ref_void_func *> create;
		Array<void_ref_func *> destroy;
		Array<bool_ref_func *> contains;
		Array<loading_func *>  load;
		Array<loading_func *>  unload;
		Array<loading_func *>  update;
	};
	static State state;
	static VTable vtable;
	static Strings_Storage strings;

	// strings API
	static u32 store_string(cstring data, u32 length);
	static u32 get_resource(cstring data, u32 length);
	static cstring get_string(u32 id);

	// system API
	static void update(void);
	static void reset_system(u32 type);

	// types API
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
