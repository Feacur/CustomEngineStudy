#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
#endif

#if !defined(CUSTOM_SHIPPING)
	void log_last_error(cstring source);
	#define LOG_LAST_ERROR() log_last_error(CUSTOM_FILE_AND_LINE)
#else
	#define LOG_LAST_ERROR() (void)0
#endif

//
// API implementation
//

static LONGLONG platform_get_file_size(HANDLE handle);
static DWORD platform_read_file(HANDLE handle, LPVOID buffer, LONGLONG to_read);

namespace custom {
namespace file {

// CompareFileTime

bool exists(cstring path) {
	WIN32_FIND_DATA findFileData;
	HANDLE handle = FindFirstFile(path, &findFileData);
	if (handle == INVALID_HANDLE_VALUE) {
		LOG_LAST_ERROR();
		return false;
	}

	FindClose(handle);
	return true;
}

void read(cstring path, Array<u8> & buffer) {
	HANDLE handle = CreateFile(
		path,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (handle == INVALID_HANDLE_VALUE) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to open file: %s", path);
		return;
	}

	// @Note: allocate an additional byte for potential '\0'
	LONGLONG file_size = platform_get_file_size(handle);
	buffer.set_capacity((u32)file_size + 1);
	if (buffer.data) {
		buffer.count = (u32)platform_read_file(handle, buffer.data, file_size);
		if (buffer.count != file_size) {
			buffer.set_capacity(0);
			CUSTOM_ASSERT(false, "failed to read file: %s;\n    read %d out of %d bytes", path, buffer.count, buffer.capacity);
		}
	}

	CloseHandle(handle);
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
