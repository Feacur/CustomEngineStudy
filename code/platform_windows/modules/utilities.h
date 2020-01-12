#include <direct.h> // _chdir

inline LPVOID allocate_memory(size_t size_in_bytes) {
	return VirtualAlloc(
		0,                        // LPVOID lpAddress
		(SIZE_T)size_in_bytes,    // SIZE_T dwSize
		MEM_RESERVE | MEM_COMMIT, // DWORD  flAllocationType
		PAGE_READWRITE            // DWORD  flProtect
	);
}

inline void free_memory(LPVOID memory) {
	VirtualFree(
		memory,     // LPVOID lpAddress
		0,          // SIZE_T dwSize
		MEM_RELEASE // DWORD  dwFreeType
	);
}

void log_last_error() {
	auto error = GetLastError();
	if (!error) { return; }

	LPTSTR messageBuffer = NULL;
	size_t size = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error,
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
		(LPTSTR)&messageBuffer, 0,
		NULL
	);

	if (size) {
		printf("0x%x: %s\n", error, messageBuffer);
		LocalFree(messageBuffer);
		return;
	}

	printf("0x%x\n", error);
}

void win_main_show_console() {
	if (AllocConsole()) {
		FILE * file_stdout = NULL;
		freopen_s(&file_stdout, "CONOUT$", "w", stdout);
		
		FILE * file_stderr = NULL;
		freopen_s(&file_stderr, "CONERR$", "w", stderr);
		
		FILE * file_stdin = NULL;
		freopen_s(&file_stdin, "CONIN$", "r", stdin);
	}
	// FreeConsole();
}

void normalize_directory(cstring exe_path) {
	cstring exe_name = strrchr(exe_path, '/');
	size_t folder_path_length = exe_name - exe_path;

	char folder_path[MAX_PATH] = {};
	// memset(folder_path, 0, sizeof(folder_path));
	strncpy_s(folder_path, sizeof(folder_path), exe_path, folder_path_length);

	_chdir(folder_path);
}
