#include "engine/core/code.h"
#include "engine/debug/log.h"

int main(int argc, char * argv[]) {
	CUSTOM_TRACE("format %d %s", 1, "trace message");
	CUSTOM_INFO("format %d %s", 2, "information");
	CUSTOM_WARN("format %d %s", 3, "warning");
	CUSTOM_ERROR("format %d %s", 4, "error message");
	// CUSTOM_ASSERT(false, "format %d %s", 5, "assertion");

	getchar();
	return 0;
}
