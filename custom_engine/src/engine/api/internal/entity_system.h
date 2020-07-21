#pragma once
#include "engine/core/collection_types.h"

namespace custom {

//
// reference
//

template<typename T>
struct Ref
{
	u32 id, gen;

	Ref();
	Ref(u32 id, u32 gen);
	Ref(T const * instance);
	~Ref() = default;

	Ref<T> & operator=(T const * instance);
	bool operator==(Ref<T> const & other);
	T * operator->();
};

//
// pool
//

struct Ref_Pool_Base
{
	virtual void destroy_safe(u32 id, u32 gen) = 0;
};

// @Todo: might want to dynamically init pools should the code be used from a DLL?
//        not quite relates to the pool itself, but definitely to Ref<T> and
//        types/places that make use of it
template<typename T>
struct Ref_Pool : public Ref_Pool_Base
{
	Ref_Pool();
	~Ref_Pool() = default;

	// SoA data
	Array<T> instances; // sparse; count indicates the last active object
	Array<u32> gens;    // sparse; count indicates the last active object
	Array<b8> active;   // sparse; count indicates the last active object

	// in-between data
	Array<u32> gaps;

	// API
	Ref<T> create();
	void destroy(Ref<T> ref);
	bool check_active(u32 id) { return id && (id < active.count) && active[id]; };
	bool contains(u32 id, u32 gen) { return check_active(id) && (gens[id] == gen); };

	// World API
	void destroy_safe(u32 id, u32 gen) override;

	// Ref<T> API
	u32 get_id(T const * instance) const { return (u32)(instance - instances.data); }
	T * get_instance(u32 id) { return &instances[id]; }
	u32 get_gen(u32 id) const { return gens[id]; }
};

//
// entity
//

struct Plain_Ref
{
	u32 id, gen;
};

struct Entity
{
	u8 data; // @Note: explicit dummy data of a single byte size

	// components
	static Array<Ref_Pool_Base *> component_pools;
	static Array<Plain_Ref> components; // sparse

	template<typename T> void add_component();
	template<typename T> void remove_component();
	template<typename T> bool has_component() const;
	template<typename T> Ref<T> get_component();

	// Ref<T> API, creation
	static Ref_Pool<Entity> pool;
	static Ref<Entity> create();
	static void destroy(Ref<Entity> ref);
};

}