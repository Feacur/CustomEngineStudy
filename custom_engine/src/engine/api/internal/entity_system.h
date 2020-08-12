#pragma once
#include "reference.h"

//
// entity components
//

namespace custom {

#define REF_VOID_FUNC(ROUTINE_NAME) Ref ROUTINE_NAME(void)
typedef REF_VOID_FUNC(ref_void_func);

#define VOID_REF_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Ref const & ref)
typedef VOID_REF_FUNC(void_ref_func);

#define BOOL_REF_FUNC(ROUTINE_NAME) bool ROUTINE_NAME(Ref const & ref)
typedef BOOL_REF_FUNC(bool_ref_func);

}

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
	static void destroy(Entity const & entity);
	inline bool exists(void) const { return generations.contains(*this); }

	// components API
	static Array<ref_void_func *> component_constructors;
	static Array<bool_ref_func *> component_containers;
	static Array<void_ref_func *> component_destructors;
	static Array<Ref> components; // sparse

	Ref  add_component(u32 type);
	void rem_component(u32 type);
	bool has_component(u32 type) const;
	Ref  get_component(u32 type) const;

	template<typename T> RefT<T> add_component(void);
	template<typename T> void    rem_component(void);
	template<typename T> bool    has_component(void) const;
	template<typename T> RefT<T> get_component(void) const;
};

}
