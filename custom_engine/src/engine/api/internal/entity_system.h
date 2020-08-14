#pragma once
#include "reference.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

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

	// API
	static Entity create(void);
	void destroy(void);
	inline bool exists(void) const { return generations.contains(*this); }

	// components API
	static Array<ref_void_func *> component_constructors;
	static Array<void_ref_func *> component_destructors;
	static Array<bool_ref_func *> component_containers;
	static Array<Ref> components; // sparse

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
