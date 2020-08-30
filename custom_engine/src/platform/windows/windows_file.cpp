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

static FILETIME platform_to_file_time(u64 value);
static LONGLONG platform_get_file_size(HANDLE handle);
static DWORD platform_read_file(HANDLE handle, LPVOID buffer, LONGLONG to_read);

namespace custom {
namespace file {

u64 get_time(cstring path) {
	WIN32_FIND_DATA findFileData;
	HANDLE handle = FindFirstFile(path, &findFileData);
	if (handle == INVALID_HANDLE_VALUE) {
		LOG_LAST_ERROR();
		return 0;
	}

	ULARGE_INTEGER large;
	large.LowPart  = findFileData.ftLastWriteTime.dwLowDateTime;
	large.HighPart = findFileData.ftLastWriteTime.dwHighDateTime;

	FindClose(handle);
	return (u64)large.QuadPart;
}

void read(cstring path, Array<u8> & buffer) {
	DWORD desiredAccess = GENERIC_READ;
	DWORD shareMode     = FILE_SHARE_READ;
	HANDLE handle = CreateFile(
		path,
		desiredAccess, shareMode, NULL, OPEN_EXISTING,
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

void watch_init(cstring path, bool subtree) {
	/*
	DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	HANDLE handle = FindFirstChangeNotification(path, subtree, notifyFilter);
	if (handle == INVALID_HANDLE_VALUE) {
		LOG_LAST_ERROR();
		return;
	}

	if (FindNextChangeNotification(handle)) {

	}

	FindCloseChangeNotification(handle);
	*/
}

void watch_shutdown(void) {

}

/*
void poll_callback(
	DWORD dwErrorCode,
	DWORD dwNumberOfBytesTransfered,
	LPOVERLAPPED lpOverlapped
) {
	CUSTOM_ASSERT(false, "");
}

void poll(cstring path, bool subtree) {
	DWORD desiredAccess = FILE_LIST_DIRECTORY;
	DWORD shareMode     = FILE_SHARE_READ | FILE_SHARE_WRITE;
	HANDLE handle = CreateFile(
		path,
		desiredAccess, shareMode, NULL, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE) {
		LOG_LAST_ERROR();
		return;
	}

	FILE_NOTIFY_INFORMATION buffer[1024];
	DWORD bytesReturned;

	DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE
		| FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME;
	while (true) {
		OVERLAPPED overlapped;
		if (ReadDirectoryChangesW(
			handle,
			(void *)buffer,
			(DWORD)sizeof(buffer),
			subtree,
			notifyFilter,
			&bytesReturned,
			&overlapped, &poll_callback
		) == 0) {
			LOG_LAST_ERROR();
		}
		break;
	}

	CloseHandle(handle);
}
*/

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
