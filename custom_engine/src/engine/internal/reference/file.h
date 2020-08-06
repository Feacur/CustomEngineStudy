#include "engine/core/collection_types.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
// #include "engine/impl/array.h"

#include <stdio.h>

namespace custom {
namespace file {

static long get_file_size(FILE * file) {
	fseek (file, 0, SEEK_END);
	long value = ftell(file); // @Note: not a size_t here
	rewind(file); // fseek (file, 0, SEEK_SET);
	return value;
}

void read_stdio(cstring path, Array<u8> & buffer) {
	FILE * file = fopen(path, "rb");
	if (!file) {
		CUSTOM_ASSERT(false, "failed to open file: %s", path);
		return;
	}

	long file_size = get_file_size(file);
	buffer.set_capacity((u32)file_size);
	if (buffer.data) {
		buffer.count = (u32)fread(buffer.data, sizeof(u8), file_size, file);
		CUSTOM_ASSERT(buffer.count == buffer.capacity, "failed to read file: %s;\n\tread %d out of %d bytes", path, buffer.count, buffer.capacity);
	}

	fclose(file);
}

}}
