namespace custom {
namespace graphics {

template<typename T> constexpr static inline Data_Type get_data_type(void) { return Data_Type::None; }
#define GDT_IMPL(T) template<> constexpr inline Data_Type get_data_type<T>(void) { return Data_Type::T; }
	GDT_IMPL(unit_id);
	GDT_IMPL(s8); GDT_IMPL(s16); GDT_IMPL(s32);
	GDT_IMPL(u8); GDT_IMPL(u16); GDT_IMPL(u32);
	GDT_IMPL(r32); GDT_IMPL(r64);
	GDT_IMPL(vec2); GDT_IMPL(vec3); GDT_IMPL(vec4);
	GDT_IMPL(ivec2); GDT_IMPL(ivec3); GDT_IMPL(ivec4);
	GDT_IMPL(uvec2); GDT_IMPL(uvec3); GDT_IMPL(uvec4);
	GDT_IMPL(mat2); GDT_IMPL(mat3); GDT_IMPL(mat4);
#undef GDT_IMPL

}}

namespace custom {
namespace loader {

template<typename T>
static void write_data_array(custom::Array<T> const & data);

void mesh(RefT<Mesh_Asset> const & ref) {
	if (graphics::mark_pending_mesh(ref.id)) { return; }

	Mesh_Asset const * asset = ref.get_fast();
	if (!asset->attributes.count) { return; }
	if (!asset->vertices.count) { return; }
	if (!asset->indices.count) { return; }

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(ref.id);
	bc->write((u8)2);
	bc->write((b8)false); bc->write(asset->vfrequency); bc->write(asset->vaccess);
	bc->write(graphics::Data_Type::r32); bc->write(asset->vertices.count); bc->write(asset->vertices.count);
	bc->write_sized_array(asset->attributes);
	bc->write((b8)true); bc->write(asset->ifrequency); bc->write(asset->iaccess);
	bc->write(graphics::Data_Type::u32); bc->write(asset->indices.count); bc->write(asset->indices.count);
	bc->write((u32)0);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write(ref.id);
	bc->write((u8)2);
	bc->write((u32)0); write_data_array(asset->vertices);
	bc->write((u32)0); write_data_array(asset->indices);
}

}}

//
// utilities
//

namespace custom {
namespace loader {

template<typename T>
static void write_data_array(custom::Array<T> const & data) {
	bc->write(graphics::get_data_type<T>());
	bc->write_sized_array(data);
}

}}
