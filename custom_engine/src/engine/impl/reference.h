#pragma once
#include "engine/api/internal/reference.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

//
// pool
//

namespace custom {

template<typename T>
Ref_PoolT<T>::Ref_PoolT(void) : Ref_Pool(sizeof(T)) {}

}
