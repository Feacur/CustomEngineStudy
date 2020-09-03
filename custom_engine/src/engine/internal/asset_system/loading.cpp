#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/internal/names_lookup.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/bytecode.h"
#include "engine/impl/asset_system.h"

#include <new>

//
//
//

namespace custom {
namespace loader {

static Bytecode * bc = NULL;

void init(Bytecode * bytecode) {
	bc = bytecode;
}

}}

// namespace custom {
// namespace graphics {
// 
// template<typename T> constexpr static inline Data_Type get_data_type(void) { return Data_Type::None; }
// #define DATA_TYPE_IMPL(T) template<> constexpr inline Data_Type get_data_type<T>(void) { return Data_Type::T; }
// #include "engine/registry_impl/data_type.h"
// 
// }}

//
// Shader_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Shader_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Shader_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	// new (asset) Shader_Asset;
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Allocate_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
	
	custom::loader::bc->write(graphics::Instruction::Load_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_unload<Shader_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Shader_Asset * asset = refT.get_fast();

	//
	asset->~Shader_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_update<Shader_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Shader_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Shader);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Allocate_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
	
	custom::loader::bc->write(graphics::Instruction::Load_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
}

}}

//
// Texture_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Texture_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Texture_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	// new (asset) Texture_Asset;
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Allocate_Texture);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Load_Texture);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_unload<Texture_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Texture_Asset * asset = refT.get_fast();

	//
	asset->~Texture_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Texture);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_update<Texture_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Texture_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Texture);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Allocate_Texture);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Load_Texture);
	custom::loader::bc->write((Ref &)asset_ref);
}

}}

//
// Mesh_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Mesh_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Mesh_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	new (asset) Mesh_Asset;
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Allocate_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Load_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_unload<Mesh_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Mesh_Asset * asset = refT.get_fast();

	//
	asset->~Mesh_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_update<Mesh_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Mesh_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Allocate_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Load_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);
}

}}

//
// Prefab_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Prefab_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "prefab asset doesn't exist"); return; }

	RefT<Prefab_Asset> & refT = (RefT<Prefab_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Prefab_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	// new (asset) Prefab_Asset;
	asset->update(file);

	// @Note: parsing a prefab creates entities immediately
}

template<> LOADING_FUNC(asset_pool_unload<Prefab_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "prefab asset doesn't exist"); return; }

	RefT<Prefab_Asset> & refT = (RefT<Prefab_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Prefab_Asset * asset = refT.get_fast();

	//
	asset->entity.destroy();
	asset->entity = {custom::empty_ref};
}

template<> LOADING_FUNC(asset_pool_update<Prefab_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "prefab asset doesn't exist"); return; }

	RefT<Prefab_Asset> & refT = (RefT<Prefab_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Prefab_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	// @Todo: update descendants, too?
	asset->entity.destroy();
	asset->update(file);

	// @Note: parsing a prefab creates entities immediately
}

}}

//
// Config_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Config_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "prefab asset doesn't exist"); return; }

	RefT<Config_Asset> & refT = (RefT<Config_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Config_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	new (asset) Config_Asset;
	asset->update(file);

	// @Note: config is a passive data storage
}

template<> LOADING_FUNC(asset_pool_unload<Config_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "config asset doesn't exist"); return; }

	RefT<Config_Asset> & refT = (RefT<Config_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Config_Asset * asset = refT.get_fast();

	//
	asset->entries.~Array();
}

template<> LOADING_FUNC(asset_pool_update<Config_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "config asset doesn't exist"); return; }

	RefT<Config_Asset> & refT = (RefT<Config_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	Config_Asset * asset = refT.get_fast();

	//
	cstring path = asset_ref.get_path();
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	asset->update(file);

	// @Note: config is a passive data storage
}

}}
