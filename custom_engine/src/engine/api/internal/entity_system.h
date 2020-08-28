#pragma once
#include "strings_storage.h"
#include "reference.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

// @Note: two modes:
//        - dense: SoA of {component ref, type, entity id}
//        - sparse: each enity always has N components, some of them active;
//          packed into blocks inside `components` array
// #define ENTITY_COMPONENTS_DENSE

namespace custom {
	// @Forward
	struct Entity;
}

//
//
//

namespace custom {

#define FROM_TO_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Ref const & from, Ref & to)
typedef FROM_TO_FUNC(from_to_func);

#define SERIALIZATION_READ_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Ref & ref, cstring * source, cstring const end)
typedef SERIALIZATION_READ_FUNC(serialization_read_func);

}

//
// entity
//

namespace custom {

template<typename T> struct Component_Registry { static u32 type; };

// @Todo: separate Entity_System container from Entity? and make the system container an instance?
struct Entity : public Ref
{
	// entities
	static Gen_Pool      generations;
	static Array<Entity> instances;
	static Strings_Storage strings;

	// components
	static Array<Ref> components;

	#if defined(ENTITY_COMPONENTS_DENSE)
	static Array<u32> component_types;
	static Array<u32> component_entity_ids;
	#endif

	// strings API
	static u32 store_string(cstring data, u32 length);
	static cstring get_string(u32 id);

	// API
	static Entity create(bool is_instance);
	static Entity serialization_read(Array<u8> const & file, bool is_instance);
	void destroy(void);
	Entity copy() const;
	inline bool exists(void) const { return generations.contains(*this); }

	// components API
	static Array<ref_void_func *> component_constructors;
	static Array<void_ref_func *> component_destructors;
	static Array<bool_ref_func *> component_containers;
	static Array<from_to_func *>  component_copiers;
	static Array<serialization_read_func *> component_serialization_readers;

	Ref  add_component(u32 type);
	void rem_component(u32 type);
	Ref  get_component(u32 type) const;
	bool has_component(u32 type) const;

	template<typename T> RefT<T> add_component(void);
	template<typename T> void    rem_component(void);
	template<typename T> RefT<T> get_component(void) const;
	template<typename T> bool    has_component(void) const;
};

// @Note: experimental, mimics Asset; in case one needs fully self-contained ref
struct Component
{
	Ref ref;
	Entity entity;
	u32 type;

	Ref get(void);
	bool exists(void) const;
	void destroy(void);
};

// @Note: experimental, mimics Asset_RefT; in case one needs fully self-contained ref
template<typename T> struct Component_RefT
{
	RefT<T> ref;
	Entity entity;

	RefT<T> get(void);
	bool exists(void) const;
	void destroy(void);
};

}
