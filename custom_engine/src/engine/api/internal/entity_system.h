#pragma once
#include "reference.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

// @Note: two modes:
//        - dense: SoA of {component ref, type, entity id}
//        - sparse: each enity always has N components, some of them active;
//          packed into blocks inside `components` array
// #define ENTITY_COMPONENTS_DENSE

//
// entity
//

namespace custom {

template<typename T> struct Component_Registry { static u32 type; };

struct Entity : public Ref
{
	// entities
	static Gen_Pool generations;
	static Array<Entity> instances;

	// components
	static Array<Ref> components;

	#if defined(ENTITY_COMPONENTS_DENSE)
	static Array<u32> component_types;
	static Array<u32> component_entity_ids;
	#endif

	// API
	static Entity create(bool is_instance);
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

}
