namespace custom {
namespace loader {

template<typename T>
static void write_data_array(custom::Array<T> const & data);

void mesh(RefT<Mesh_Asset> const & ref) {
	if (graphics::mark_pending_mesh(ref.id)) { return; }

	Mesh_Asset const * asset = ref.get_fast();
	if (!asset->buffers.count) { return; }

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write((Ref const &)ref);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write((Ref const &)ref);
}

}}
