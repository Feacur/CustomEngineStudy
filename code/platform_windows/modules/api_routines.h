PLATFORM_READ_FILE(platform_read_file) {
	HANDLE file = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(file == INVALID_HANDLE_VALUE) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't open file");
		return 0;
	}
	
	LARGE_INTEGER file_size;
	if(!GetFileSizeEx(file, &file_size)) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't get file size");
		CloseHandle(file);
		return 0;
	}
	
	DWORD dword_file_size = (DWORD)file_size.QuadPart;
	u8 * bytes = allocate_data(memory, dword_file_size);
	if (!bytes) {
		log_last_error();
		CloseHandle(file);
		return 0;
	}
	
	DWORD bytes_read;
	if(!ReadFile(file, bytes, dword_file_size, &bytes_read, 0)) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't read file");
		deallocate_from_top(memory, dword_file_size);
		CloseHandle(file);
		return 0;
	}
	
	CloseHandle(file);

	return bytes;
}

PLATFORM_ALLOCATE_MEMORY(platform_allocate_memory) {
	HANDLE heap_handle = GetProcessHeap();
	return HeapAlloc(
		heap_handle,          // HANDLE hHeap
		NULL,                 // DWORD dwFlags
		(SIZE_T)size_in_bytes // SIZE_T dwBytes
	);
}

PLATFORM_REALLOCATE_MEMORY(platform_reallocate_memory) {
	HANDLE heap_handle = GetProcessHeap();
	return HeapReAlloc(
		heap_handle,          // HANDLE hHeap
		NULL,                 // DWORD dwFlags
		memory,               // LPVOID lpMem
		(SIZE_T)size_in_bytes // SIZE_T dwBytes
	);
}

PLATFORM_FREE_MEMORY(platform_free_memory) {
	HANDLE heap_handle = GetProcessHeap();
	HeapFree(
		heap_handle, // HANDLE hHeap
		NULL,        // DWORD dwFlags
		memory       // LPVOID lpMem
	);
}
