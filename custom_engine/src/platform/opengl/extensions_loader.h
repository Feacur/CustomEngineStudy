#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/debug/log.h"
#include "engine/impl/array.h"

namespace opengl {

static bool contains_full_word(cstring container, cstring value) {
	CUSTOM_ASSERT(value, "value is nullptr");
	CUSTOM_ASSERT(*value != '\0', "value is empty");
	CUSTOM_ASSERT(!strchr(value, ' '), "value contains spaces: '%s'", value);

	cstring start = container;
	while (true)
	{
		cstring where = strstr(start, value);
		if (!where) { return false; }

		cstring terminator = where + strlen(value);
		if (where == start || *(where - 1) == ' ') {
			if (*terminator == ' ' || *terminator == '\0')
				break;
		}

		start = terminator;
	}

	return true;
}

static void allocate_extensions_string(custom::Array<char> & buffer) {
	GLint extensions_count = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extensions_count);
	buffer.ensure_capacity(extensions_count * 16);
	for(u32 index = 0; index < (u32)extensions_count; index++) {
		cstring value = (cstring)glGetStringi(GL_EXTENSIONS, index);
		u32 length = (u32)strlen(value);
		buffer.push_range(value, length);
		buffer.push(' ');
	}
	if (buffer.count > 0) { --buffer.count; }
	buffer.push('\0');
}

typedef void * load_func(cstring name);

#define CHECK_EXTENSION(name)\
	bool const name = contains_full_word(buffer.data, "GL_" #name)

#define LOAD_EXTENSION(func, name)\
	if (!func && name) {\
		glad_##func = (decltype(func))load(#func);\
		CUSTOM_TRACE("load '" #func "' as an extension '" #name "'");\
	}\

void load_extensions(load_func * load) {
	custom::Array<char> buffer;
	allocate_extensions_string(buffer);
	CHECK_EXTENSION(ARB_clip_control);
	CHECK_EXTENSION(ARB_debug_output);
	LOAD_EXTENSION(glClipControl, ARB_clip_control);
	LOAD_EXTENSION(glDebugMessageCallback, ARB_debug_output);
	LOAD_EXTENSION(glDebugMessageControl, ARB_debug_output);
}
#undef CHECK_EXTENSION
#undef LOAD_EXTENSION

}
