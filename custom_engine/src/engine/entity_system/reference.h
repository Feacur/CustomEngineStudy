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

	Ref();
	Ref(u32 id, u32 gen);
	Ref(T const * instance);
	~Ref() = default;

	Ref<T> & operator=(T const * instance);
	bool operator==(Ref<T> const & other);
	T * operator->();
};

template<typename T>
Ref<T>::Ref()
	: id(0)
	, gen(0)
{ }

template<typename T>
Ref<T>::Ref(u32 id, u32 gen)
	: id(id)
	, gen(gen)
{ }

template<typename T>
Ref<T>::Ref(T const * instance) {
	if (instance) {
		id = T::pool.get_id(instance);
		gen = T::pool.get_gen(id);
	}
	else {
		id = 0;
	}
}

template<typename T>
Ref<T> & Ref<T>::operator=(T const * instance) {
	if (instance) {
		id = T::pool.get_id(instance);
		gen = T::pool.get_gen(id);
	}
	else {
		id = 0;
	}
	return *this;
}

template<typename T>
bool Ref<T>::operator==(Ref<T> const & other) {
	return id == other.id && gen = other.gen;
}

template<typename T>
T * Ref<T>::operator->() {
	return T::pool.contains(id, gen) ? T::pool.get_instance(id) : NULL;
}

struct Ref_Pool_Base
{
	virtual void destroy_safe(u32 id, u32 gen) = 0;
};

template<typename T>
struct Ref_Pool : public Ref_Pool_Base
{
	Ref_Pool();
	~Ref_Pool() = default;

	// SoA data
	Array<T> instances;
	Array<u32> gens;
	Array<b8> active;

	// in-between data
	Array<u32> gaps;

	// API
	Ref<T> create();
	void destroy(Ref<T> ref);
	bool contains(u32 id, u32 gen) { return id && active[id] && (gens[id] == gen); };

	// World API
	void destroy_safe(u32 id, u32 gen) override;

	// Ref<T> API
	u32 get_id(T const * instance) const { return (u32)(instance - instances.data); }
	T * get_instance(u32 id) { return &instances[id]; }
	u32 get_gen(u32 id) const { return gens[id]; }
};

template<typename T>
Ref_Pool<T>::Ref_Pool() {
	Ref<T> null_instance = create();
}

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
		active.push();
	}
	active[id] = true;
	return { id, gens[id] };
}

template<typename T>
void Ref_Pool<T>::destroy(Ref<T> ref) {
	CUSTOM_ASSERT(ref.gen == gens[ref.id], "destroying null data");
	if (ref.id == instances.count - 1) {
		instances.pop();
		gens.pop();
		active.pop();
	}
	else {
		gaps.push(ref.id);
	}
	active[ref.id] = false;
	++gens[ref.id];
}

template<typename T>
void Ref_Pool<T>::destroy_safe(u32 id, u32 gen) {
	if (contains(id, gen)) {
		destroy({ id, gen });
	}
}

}
