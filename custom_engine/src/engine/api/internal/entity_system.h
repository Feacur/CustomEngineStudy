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
// universal access
//

namespace custom {

#define ENTITY_FROM_TO_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Entity & entity, Ref const & from, Ref & to)
typedef ENTITY_FROM_TO_FUNC(entity_from_to_func);

#define ENTITY_LOADING_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Entity & entity, Ref & ref, bool only_component)
typedef ENTITY_LOADING_FUNC(entity_loading_func);

#define SERIALIZATION_READ_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Entity & entity, Ref & ref, cstring * source)
typedef SERIALIZATION_READ_FUNC(serialization_read_func);

}

//
// entity
//

namespace custom {

template<typename T> struct Component_Registry { static u32 type; };
template<typename T> u32 Component_Registry<T>::type;

// @Todo: separate Entity_System container from Entity? and make the system container an instance?
struct Entity : public Ref
{
	struct State {
		// entities
		Gen_Pool        generations;
		Array<Entity>   instances;

		// components
		Array<Ref> components;

		#if defined(ENTITY_COMPONENTS_DENSE)
		Array<u32> component_types;
		Array<u32> component_entity_ids;
		#endif
	};
	struct VTable {
		Array<ref_void_func *> create;
		Array<void_ref_func *> destroy;
		Array<bool_ref_func *> contains;
		Array<entity_from_to_func *> copy;
		Array<entity_loading_func *> load;
		Array<entity_loading_func *> unload;
		Array<serialization_read_func *> serialization_read;
	};
	static State state;
	static VTable vtable;
	static Strings_Storage strings;

	// strings API
	static u32 store_string(cstring data, u32 length);
	static cstring get_string(u32 id);

	// system API
	static void reset_system(void);

	// entity API
	static Entity create(bool is_instance);
	void serialization_read(cstring * source);
	void override_with(Entity const & source);
	void destroy(void);
	bool is_instance() const;
	Entity copy(bool force_instance) const;
	void promote_to_instance(void);
	inline bool exists(void) const { return state.generations.contains(*this); }

	// components API
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
