#include "custom_pch.h"
#include "engine/api/file.h"
#include "engine/core/collection_types.h"
// #include "engine/impl/array.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
#endif

#if !defined(CUSTOM_SHIPPING)
	extern void log_last_error(cstring source);
	#define LOG_LAST_ERROR() log_last_error(CUSTOM_FILE_AND_LINE)
#else
	#define LOG_LAST_ERROR()
#endif

//
// API implementation
//

static LONGLONG platform_get_file_size(HANDLE handle);
static DWORD platform_read_file(HANDLE handle, LPVOID buffer, LONGLONG to_read);

namespace custom {
namespace file {

void read(cstring path, Array<u8> & buffer) {
	HANDLE file = CreateFile(
		path,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (file == INVALID_HANDLE_VALUE) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to open file: %s", path);
		return;
	}

	LONGLONG file_size = platform_get_file_size(file);
	buffer.set_capacity((u32)file_size);
	if (buffer.data) {
		buffer.count = (u32)platform_read_file(file, buffer.data, file_size);
		CUSTOM_ASSERT(buffer.count == buffer.capacity, "failed to read file: %s;\n\tread %d out of %d bytes", path, buffer.count, buffer.capacity);
	}

	CloseHandle(file);
}

}}

//
// platform implementation
//

static LONGLONG platform_get_file_size(HANDLE handle) {
	LARGE_INTEGER value;
	if (!GetFileSizeEx(handle, &value)) {
		LOG_LAST_ERROR();
	}
	return value.QuadPart;
}

static DWORD platform_read_file(HANDLE handle, LPVOID buffer, LONGLONG to_read) {
	DWORD read_size;
	if (!ReadFile(handle, buffer, (DWORD)to_read, &read_size, NULL)) {
		LOG_LAST_ERROR();
	}
	return read_size;
}
