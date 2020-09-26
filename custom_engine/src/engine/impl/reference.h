#pragma once
#include "engine/api/internal/reference.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

//
// pool
//

namespace custom {

template<typename T>
RefT<T> Ref_PoolT<T>::create(void) {
	if (generations.gaps.count == 0) { instances.push(); }
	Ref ref = generations.create();
	return {ref};
}

template<typename T>
void Ref_PoolT<T>::destroy(Ref const & ref) {
	generations.destroy(ref);
	if (ref.id == instances.count - 1) { instances.pop(); }
}

}
