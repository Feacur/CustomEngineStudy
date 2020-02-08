#pragma once
#include "engine/impl/array.h"

#include <new>

namespace custom {

//
// reference
//

template<typename T>
struct Ref
{
	u32 id, gen;

	Ref()
		: id(0)
		, gen(0)
	{ }

	Ref(u32 id, u32 gen)
		: id(id)
		, gen(gen)
	{ }

	Ref(T const * instance)
	{
		if (instance) {
			id = T::pool.get_id(instance);
			gen = T::pool.get_gen(id);
		}
		else {
			id = 0;
		}
	}

	Ref<T> & operator=(T const * instance)
	{
		if (instance) {
			id = T::pool.get_id(instance);
			gen = T::pool.get_gen(id);
		}
		else {
			id = 0;
		}
		return *this;
	}

	bool operator==(Ref<T> const & other)
	{
		return id == other.id && gen = other.gen;
	}

	T * operator->()
	{
		if (!id) { return NULL; }
		T * instance = T::pool.get_instance(id);
		return gen == T::pool.get_gen(id) ? instance : NULL;
	}
};

struct Ref_Pool_Base
{
	virtual void destroy(u32 id, u32 gen) = 0;
};

template<typename T>
struct Ref_Pool : public Ref_Pool_Base
{
	Ref_Pool()
	{
		Ref<T> null_instance = create();
	}

	// SoA data
	Array<T> instances;
	Array<u32> gens;

	// in-between data
	Array<u32> gaps;

	// API
	Ref<T> create();
	void destroy(Ref<T> ref);
	void destroy(u32 id, u32 gen) override;
	bool contains(u32 id, u32 gen) { gens[id] == gen; };

	// Ref<T> API
	u32 get_id(T const * instance) const { return (u32)(instance - instances.data); }
	T * get_instance(u32 id) { return &instances[id]; }
	u32 get_gen(u32 id) const { return gens[id]; }
};

template<typename T>
Ref<T> Ref_Pool<T>::create() {
	u32 id;
	if (gaps.count > 0) {
		gaps.pop();
		id = gaps[gaps.count];
	}
	else {
		id = instances.count;
		instances.push();
		gens.push();
	}
	return { id, gens[id] };
}

template<typename T>
void Ref_Pool<T>::destroy(Ref<T> ref) {
	CUSTOM_ASSERT(ref.gen == gens[ref.id], "destroying null data");
	if (ref.id == instances.count - 1) {
		instances.pop();
		gens.pop();
	}
	else {
		gaps.push(ref.id);
	}
	++gens[ref.id];
}

template<typename T>
void Ref_Pool<T>::destroy(u32 id, u32 gen) {
	destroy({ id, gen });
}

}
