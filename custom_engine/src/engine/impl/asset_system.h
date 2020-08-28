#pragma once
#include "engine/api/internal/asset_system.h"

//
// universal access
//

namespace custom {
namespace loading {

template<typename T> LOADING_FUNC(asset_pool_load);
template<typename T> LOADING_FUNC(asset_pool_unload);

}}

//
// asset
//

namespace custom {

template<typename T>
Asset_RefT<T> Asset::add(u32 resource) {
	return {add(Asset_Registry<T>::type, resource).ref, resource};
}

template<typename T>
Asset_RefT<T> Asset::get(u32 resource) {
	return {get(Asset_Registry<T>::type, resource).ref, resource};
}

template<typename T>
bool Asset::has(u32 resource) {
	return has(Asset_Registry<T>::type, resource);
}

template<typename T>
cstring Asset::get_path(Asset_RefT<T> const & ref) {
	return Asset{ref.ref, ref.resource, Asset_Registry<T>::type}.get_path();
}

template<typename T>
bool Asset::exists(Asset_RefT<T> const & ref) {
	return Asset{ref.ref, ref.resource, Asset_Registry<T>::type}.exists();
}

template<typename T>
void Asset::unload(Asset_RefT<T> & ref) {
	return Asset{ref.ref, ref.resource, Asset_Registry<T>::type}.unload();
}

}
