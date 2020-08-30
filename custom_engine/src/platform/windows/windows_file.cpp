#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/debug/log.h"
#include "engine/api/internal/strings_storage.h"
#include "engine/api/platform/file.h"
#include "engine/api/platform/timer.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
#endif

#if !defined(CUSTOM_SHIPPING)
	void log_last_error(cstring source);
	#define LOG_LAST_ERROR() log_last_error(CUSTOM_FILE_AND_LINE)
#else
	#define LOG_LAST_ERROR() (void)0
#endif

#include <stdlib.h>

//
// API implementation
//

static FILETIME platform_to_file_time(u64 value);
static LONGLONG platform_get_file_size(HANDLE handle);
static DWORD platform_read_file(HANDLE handle, LPVOID buffer, LONGLONG to_read);
static DWORD WINAPI watch_change_callback(LPVOID lpParam);

namespace custom {
namespace file {

u64 get_time(cstring path) {
	WIN32_FIND_DATA find_file_data;
	HANDLE handle = FindFirstFile(path, &find_file_data);
	if (handle == INVALID_HANDLE_VALUE) {
		LOG_LAST_ERROR();
		return 0;
	}

	ULARGE_INTEGER large;
	large.LowPart  = find_file_data.ftLastWriteTime.dwLowDateTime;
	large.HighPart = find_file_data.ftLastWriteTime.dwHighDateTime;

	FindClose(handle);
	return (u64)large.QuadPart;
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

struct Watch_Change_Data {
	cstring path;
	bool subtree;
	HANDLE change_handle = INVALID_HANDLE_VALUE;
	HANDLE file_handle   = INVALID_HANDLE_VALUE;
	HANDLE thread_handle = INVALID_HANDLE_VALUE;
	DWORD  thread_id;

	Strings_Storage storage;
	volatile u64 storage_id;

	void shutdown(void) {
		if (thread_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(thread_handle);
			thread_handle = INVALID_HANDLE_VALUE;
		}
		if (file_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(file_handle);
			file_handle = INVALID_HANDLE_VALUE;
		}
		if (change_handle != INVALID_HANDLE_VALUE) {
			FindCloseChangeNotification(change_handle);
			change_handle = INVALID_HANDLE_VALUE;
		}
	}
};

static Watch_Change_Data watch_change_data;
void watch_init(cstring path, bool subtree) {
	watch_change_data.path          = path;
	watch_change_data.subtree       = subtree;
	watch_change_data.thread_handle = CreateThread(NULL, 0, watch_change_callback, &watch_change_data, 0, &watch_change_data.thread_id);
	if (watch_change_data.thread_handle == INVALID_HANDLE_VALUE) { LOG_LAST_ERROR(); }
}

void watch_update(void) {
	static u64 storage_id;

	modified.clear();
	if (storage_id != watch_change_data.storage_id) {
		// @Todo: threads synchronization
		storage_id = watch_change_data.storage_id;
		modified.values.push_range(watch_change_data.storage.values.data, watch_change_data.storage.values.count);
		modified.offsets.push_range(watch_change_data.storage.offsets.data, watch_change_data.storage.offsets.count);
		watch_change_data.storage.clear();
	}
}

void watch_shutdown(void) {
	watch_change_data.shutdown();
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

static DWORD WINAPI watch_change_callback(LPVOID lpParam) {
	typedef custom::file::Watch_Change_Data Watch_Change_Data;
	Watch_Change_Data * data = (Watch_Change_Data *)lpParam;

	// @Note: code tracks size, which is error prone; the reason is that `xcopy`
	//        triggers all the files somehow, despite the `/D` flag, which
	//        skips unchanged files
	constexpr DWORD const notify_filter = 0
		| FILE_NOTIFY_CHANGE_SIZE
		// | FILE_NOTIFY_CHANGE_FILE_NAME
		// | FILE_NOTIFY_CHANGE_DIR_NAME
		// | FILE_NOTIFY_CHANGE_LAST_WRITE
		;

	data->change_handle = FindFirstChangeNotification(data->path, data->subtree, notify_filter);
	if (data->change_handle != INVALID_HANDLE_VALUE) {
		data->file_handle = CreateFile(
			data->path,
			FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL
		);
	}

	while (data->change_handle != INVALID_HANDLE_VALUE && data->file_handle != INVALID_HANDLE_VALUE) {
		DWORD wait_status = WaitForSingleObject(data->change_handle, INFINITE);
		if (wait_status == WAIT_OBJECT_0) {
			// @Note: is being called twice, beware, account
			static FILE_NOTIFY_INFORMATION info[1024] = {};
			DWORD bytes_returned;
			BOOL read_result = ReadDirectoryChangesW(
				data->file_handle,
				(void *)info, (DWORD)sizeof(info),
				data->subtree, notify_filter,
				&bytes_returned,
				NULL, NULL
			);
			if (!read_result) { LOG_LAST_ERROR(); }

			if (bytes_returned) {
				// CUSTOM_TRACE("change (%d bytes)", bytes_returned);
				FILE_NOTIFY_INFORMATION const * next = info;
				u32 const count = (u32)(bytes_returned / sizeof(*info));
				for (u32 i = 0; i < count; ++i) {
					// @Todo: unicode paths? also for the asset system?
					static char buffer[256];
					CUSTOM_ASSERT(next->FileNameLength <= C_ARRAY_LENGTH(buffer), "out of bounds");
					wcstombs(buffer, next->FileName, next->FileNameLength);
					// CUSTOM_TRACE("system change: '%s' (%d)", buffer, next->FileNameLength);
					data->storage.store_string(buffer, next->FileNameLength);
					if (!next->NextEntryOffset) { break; }
					next = info + next->NextEntryOffset;
				}
				memset(info, 0, bytes_returned);
				// @Todo: threads synchronization; potentially ignored files?
				data->storage_id = custom::timer::get_ticks();
			}

			if (!FindNextChangeNotification(data->change_handle)) {
				CUSTOM_ASSERT(false, "wait status: can't continue");
			}
			continue;
		}

		switch (wait_status) {
			case WAIT_ABANDONED: CUSTOM_ASSERT(false, "wait status: abandoned"); break;
			case WAIT_TIMEOUT:   CUSTOM_ASSERT(false, "wait status: timeout");   break;
			case WAIT_FAILED:    CUSTOM_ASSERT(false, "wait status: failed");    break;
			default:             CUSTOM_ASSERT(false, "wait status: uknown");    break;
		}

		LOG_LAST_ERROR();
		break;
	}

	LOG_LAST_ERROR();

	data->thread_handle = INVALID_HANDLE_VALUE;
	data->shutdown();

	return 0;
}
