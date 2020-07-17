#include "custom_pch.h"
#include "engine/core/types.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
#endif

#if !defined(CUSTOM_SHIPPING)
void log_last_error(cstring source) {
	DWORD const error = GetLastError();
	if (!error) { return; }

	LPTSTR message_buffer = NULL;
	DWORD size = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error,
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
		(LPTSTR)&message_buffer, 0,
		NULL
	);

	if (size) {
		CUSTOM_MESSAGE("'0x%x' system error: %s    log source: %s", error, message_buffer, source);
		LocalFree(message_buffer);
	}
	else {
		CUSTOM_MESSAGE("'0x%x' system error: unknown\n    log source: %s", error, source);
	}
}
#endif
