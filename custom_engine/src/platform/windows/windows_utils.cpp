#include "custom_pch.h"
#include "engine/core/types.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
#endif

// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessage

#if !defined(CUSTOM_SHIPPING)
void log_last_error(cstring source) {
	DWORD const error = GetLastError();
	if (!error) { return; }

	LPTSTR message_buffer = NULL;
	DWORD size = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS
		| FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL, error,
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
		(LPTSTR)&message_buffer, 0,
		NULL
	);

	if (size) {
		CUSTOM_ERROR("'0x%x': %s", error, message_buffer);
		LocalFree(message_buffer);
	}
	else {
		CUSTOM_ERROR("'0x%x': unknown", error);
	}
	CUSTOM_MESSAGE(ANSI_TXT_GRY "  at: %s" ANSI_CLR "\n", source);
}
#endif
