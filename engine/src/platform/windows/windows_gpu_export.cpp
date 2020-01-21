#include "custom_pch.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	// #define WIN32_LEAN_AND_MEAN
	// #define NOMINMAX
	#include <Windows.h>
#endif

#if defined(CUSTOM_SHARED_LIBRARY)
	#pragma message("engine is being built as a shared library")
	#pragma message("and won't export performance variables")
#else
	extern "C" { // @Note: use discrete GPU by default
		// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
		// Global Variable NvOptimusEnablement (new in Driver Release 302)
		// Starting with the Release 302 drivers, application developers can direct the Optimus driver at runtime to use the High Performance Graphics to render any application–even those applications for which there is no existing application profile. They can do this by exporting a global variable named NvOptimusEnablement. The Optimus driver looks for the existence and value of the export. Only the LSB of the DWORD matters at this time. Avalue of 0x00000001 indicates that rendering should be performed using High Performance Graphics. A value of 0x00000000 indicates that this method should beignored.
		__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001UL;

		// https://community.amd.com/thread/169965
		// https://community.amd.com/thread/223376
		// https://gpuopen.com/amdpowerxpressrequesthighperformance/
		// Summary
		// Many Gaming and workstation laptops are available with both (1) integrated power saving and (2) discrete high performance graphics devices. Unfortunately, 3D intensive application performance may suffer greatly if the best graphics device is not selected. For example, a game may run at 30 Frames Per Second (FPS) on the integrated GPU rather than the 60 FPS the discrete GPU would enable. As a developer you can easily fix this problem by adding only one line to your executable’s source code:
		__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001UL;
		// Yes, it’s that easy. This line will ensure that the high-performance graphics device is chosen when running your application.
	}
#endif
