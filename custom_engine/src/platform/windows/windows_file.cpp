#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/meta.h"
#include "engine/core/collection_types.h"
#include "engine/debug/log.h"
#include "engine/api/internal/strings_storage.h"
#include "engine/api/platform/file.h"
#include "engine/api/platform/timer.h"
#include "engine/impl/math_bitwise.h"
#include "engine/impl/array.h"

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
static DWORD WINAPI watch_files_thread(LPVOID lpParam);

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
			CUSTOM_ASSERT(false, "failed to read file: %s;\n    read %d out of %d bytes", path, buffer.count, buffer.capacity);
			buffer.count = 0;
		}
	}

	CloseHandle(handle);
}

struct Watch_Files_Data {
	cstring path;
	bool subtree;
	HANDLE file_handle   = INVALID_HANDLE_VALUE;
	HANDLE thread_handle = INVALID_HANDLE_VALUE;
	DWORD  thread_id;

	volatile u64 storage_id;
	Strings_Storage strings;
	Array<Action> actions;

	void add_action(Action action) {
		for (u32 i = 0; i < actions.count; ++i) {
			if (actions[i].id != action.id) { continue; }
			if (actions[i].type != action.type) { continue; }
			return;
		}
		actions.push(action);
	}

	void shutdown(bool terminate) {
		if (thread_handle != INVALID_HANDLE_VALUE) {
			if (terminate) { TerminateThread(thread_handle, 0); }
			CloseHandle(thread_handle);
			thread_handle = INVALID_HANDLE_VALUE;
		}
		if (file_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(file_handle);
			file_handle = INVALID_HANDLE_VALUE;
		}
	}
};

static Watch_Files_Data watch_data;
void watch_init(cstring path, bool subtree) {
	watch_data.path          = path;
	watch_data.subtree       = subtree;
	watch_data.thread_handle = CreateThread(NULL, 0, watch_files_thread, &watch_data, 0, &watch_data.thread_id);
	if (watch_data.thread_handle == INVALID_HANDLE_VALUE) { LOG_LAST_ERROR(); }
}

void watch_update(void) {
	static u64 storage_id;

	strings.clear();
	actions.count = 0;

	if (storage_id != watch_data.storage_id) {
		// @Todo: threads synchronization
		storage_id = watch_data.storage_id;
		strings.values.push_range(watch_data.strings.values.data, watch_data.strings.values.count);
		strings.offsets.push_range(watch_data.strings.offsets.data, watch_data.strings.offsets.count);
		strings.lengths.push_range(watch_data.strings.lengths.data, watch_data.strings.lengths.count);
		actions.push_range(watch_data.actions.data, watch_data.actions.count);

		watch_data.strings.clear();
		watch_data.actions.count = 0;
	}
}

void watch_shutdown(void) {
	watch_data.shutdown(true);
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

static DWORD WINAPI watch_files_thread(LPVOID lpParam) {
	typedef custom::file::Watch_Files_Data Watch_Data;
	Watch_Data * data = (Watch_Data *)lpParam;
	
	// @Note: `FILE_NOTIFY_INFORMATION` uses flexible array member to store file names
	//        - https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-file_notify_information
	//        - must be DWORD-aligned; is this an issue, really?
	//        - manually align if using version older than C++11?
	constexpr u32 const info_size_guesstimate = sizeof(FILE_NOTIFY_INFORMATION) + 26 * sizeof(FILE_NOTIFY_INFORMATION::FileName);
	alignas(DWORD) static u8 info[info_size_guesstimate * 1024] = {};
	static custom::Array<char> buffer;

	// @Note: file system is not ideal
	//        - tracking only size might occasionally lose you changes
	//        - tracking last write is triggered by any breeze, and it's annoying
	//          ... disabling still yields file modifications somehow
	//          ... enabling it clutters the feed with unnecessary parent folder changes
	//        - file name and dir name seem to be ok
	constexpr DWORD const notify_filter = 0
		| FILE_NOTIFY_CHANGE_SIZE
		// | FILE_NOTIFY_CHANGE_LAST_WRITE
		| FILE_NOTIFY_CHANGE_FILE_NAME
		| FILE_NOTIFY_CHANGE_DIR_NAME
		;

	data->file_handle = CreateFile(
		data->path,
		FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	while (data->file_handle != INVALID_HANDLE_VALUE) {
		DWORD bytes_returned;
		BOOL read_result = ReadDirectoryChangesW(
			data->file_handle,
			(void *)info, (DWORD)sizeof(info),
			data->subtree, notify_filter,
			&bytes_returned,
			NULL, NULL
		);
		if (!read_result) {
			LOG_LAST_ERROR();
			CUSTOM_ASSERT(false, "failed to read changes");
			continue;
		}
		if (!bytes_returned) { continue; }

		u8 const * next_byte = info;
		while (true) {
			FILE_NOTIFY_INFORMATION const * next = (FILE_NOTIFY_INFORMATION const *)next_byte;
			// @Note: file name is presented in bytes
			u32 file_name_lenth = (u32)(next->FileNameLength / sizeof(*next->FileName));
			buffer.ensure_capacity(file_name_lenth);
			buffer.count = (u32)wcstombs(buffer.data, next->FileName, file_name_lenth);

			if (buffer.count != file_name_lenth) {
				CUSTOM_ASSERT(false, "failed to convert bytes;\n    read %d out of %d bytes", buffer.count, file_name_lenth);
				buffer.count = 0;
			}

			custom::file::Action_Type type = custom::file::Action_Type::None;
			switch (next->Action) {
				case FILE_ACTION_ADDED:            type = custom::file::Action_Type::Add; break;
				case FILE_ACTION_REMOVED:          type = custom::file::Action_Type::Rem; break;
				case FILE_ACTION_MODIFIED:         type = custom::file::Action_Type::Mod; break;
				case FILE_ACTION_RENAMED_OLD_NAME: type = custom::file::Action_Type::Old; break;
				case FILE_ACTION_RENAMED_NEW_NAME: type = custom::file::Action_Type::New; break;
			}

			// buffer.push('\0'); --buffer.count;
			u32 id = data->strings.store_string(buffer.data, buffer.count);
			data->add_action({id, type});

			if (!next->NextEntryOffset) { break; }
			next_byte = next_byte + next->NextEntryOffset;
		}
		// @Todo: threads synchronization; potentially ignored files?
		data->storage_id = custom::timer::get_ticks();
	}

	data->shutdown(false);
	return 0;
}

/*
namespace custom {
namespace file {

struct Watch_Change_Data {
	cstring path;
	bool subtree;
	HANDLE change_handle = INVALID_HANDLE_VALUE;
	HANDLE thread_handle = INVALID_HANDLE_VALUE;
	DWORD  thread_id;

	void shutdown(bool terminate) {
		if (thread_handle != INVALID_HANDLE_VALUE) {
			if (terminate) { TerminateThread(thread_handle, 0); }
			CloseHandle(thread_handle);
			thread_handle = INVALID_HANDLE_VALUE;
		}
		if (change_handle != INVALID_HANDLE_VALUE) {
			FindCloseChangeNotification(change_handle);
			change_handle = INVALID_HANDLE_VALUE;
		}
	}
};

}}

static DWORD WINAPI watch_change_thread(LPVOID lpParam) {
	typedef custom::file::Watch_Change_Data Watch_Data;
	Watch_Data * data = (Watch_Data *)lpParam;

	// @Note: file system is not ideal
	//        - tracking only size might occasionally lose you changes
	//        - tracking last write is triggered by any breeze, and it's annoying
	//          ... disabling still yields file modifications somehow
	//          ... enabling it clutters the feed with unnecessary parent folder changes
	//        - file name and dir name seem to be ok
	constexpr DWORD const notify_filter = 0
		| FILE_NOTIFY_CHANGE_SIZE
		// | FILE_NOTIFY_CHANGE_LAST_WRITE
		| FILE_NOTIFY_CHANGE_FILE_NAME
		| FILE_NOTIFY_CHANGE_DIR_NAME
		;

	data->change_handle = FindFirstChangeNotification(data->path, data->subtree, notify_filter);

	while (data->change_handle != INVALID_HANDLE_VALUE) {
		DWORD wait_status = WaitForSingleObject(data->change_handle, INFINITE);
		if (wait_status == WAIT_TIMEOUT) { continue; }
		if (wait_status == WAIT_FAILED) { LOG_LAST_ERROR(); CUSTOM_ASSERT(false, "failed to wait"); continue; }
		if (wait_status != WAIT_OBJECT_0) { CUSTOM_ASSERT(false, "unknown wait status: '0x%x'", wait_status); continue; }

		if (!FindNextChangeNotification(data->change_handle)) {
			LOG_LAST_ERROR();
			CUSTOM_ASSERT(false, "can't schedule next change search");
			break;
		}
	}

	data->shutdown(false);
	return 0;
}
*/
