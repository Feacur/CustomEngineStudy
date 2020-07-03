@echo off

rem https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019
rem https://docs.microsoft.com/en-us/cpp/build/reference/compiling-a-c-cpp-program?view=vs-2019
rem https://docs.microsoft.com/en-us/cpp/build/reference/linking?view=vs-2019
rem https://docs.microsoft.com/en-us/cpp/build/reference/lib-reference?view=vs-2019

chcp 65001
rem code page 65001: Unicode (UTF-8)
rem code page 1250: ANSI Central European; Central European (Windows)
rem code page 1251: ANSI Cyrillic; Cyrillic (Windows)
rem code page 1252: ANSI Latin 1; Western European (Windows)

set VSLANG=1033
rem msbuild locale 1033: English

rem read batch file arguments
set compilation_unit=%1
set target_name=%2
set kind=%3
set architecture_compiler=%4
set architecture_target=%5
set configuration=%6
set defines=%7
set libraries=%8

if [%compilation_unit%] == [] (
	echo provide compilation unit [*.cpp]
	exit /b 0
)

if [%target_name%] == [] (
	echo provide target name
	exit /b 0
)

if [%kind%] == [] (
	echo provide kind [StaticLib, SharedLib, ConsoleApp, WindowedApp]
	exit /b 0
)

if [%architecture_compiler%] == [] (
	echo provide architecture_compiler [x64, x86]
	exit /b 0
)

if [%architecture_target%] == [] (
	echo provide architecture_target [x64, x86, arm, arm64]
	exit /b 0
)

if [%configuration%] == [] (
	echo provide kind [Debug, Development, Shipping]
	exit /b 0
)

if [%defines%] == [] (
	set defines=""
)

if [%libraries%] == [] (
	set libraries=""
)

rem clean batch file arguments
set compilation_unit=%compilation_unit:"=%
set target_name=%target_name:"=%
set kind=%kind:"=%
set architecture_compiler=%architecture_compiler:"=%
set architecture_target=%architecture_target:"=%
set configuration=%configuration:"=%
set defines=%defines:"=%
set libraries=%libraries:"=%

rem declare paths
set rootdir=%~dp0.
set sourcedir=.\code
set outputdir=v1\custom\%target_name%
set target_location=.\bin\%outputdir%
set intermediate_location=.\bin-int\%outputdir%

rem //
rem // >> COMPILER
rem //
set compiler=-std:c++17
set compiler=%compiler% -I%sourcedir%

rem Program Database File Name
set compiler=%compiler% -Fd"%intermediate_location%\\"
rem Object File Name
set compiler=%compiler% -Fo"%intermediate_location%\\"
rem Name Mapfile
rem set compiler=%compiler% -Fm"%intermediate_location%\\"

set defines=%defines% -D _HAS_EXCEPTIONS=0
rem @Note: might want to enable this only locally
set defines=%defines% -D _CRT_SECURE_NO_WARNINGS
rem set defines=%defines% -D _UNICODE -D UNICODE
rem set defines=%defines% -D _MBCS

if %configuration% == Shipping (
	set defines=%defines% -D CUSTOM_SHIPPING
) else if %configuration% == Development (
	set defines=%defines% -D CUSTOM_DEVELOPMENT
) else if %configuration% == Debug (
	set defines=%defines% -D CUSTOM_DEBUG
)

if %kind% == StaticLib (
	set defines=%defines% -D CUSTOM_STATIC_LIBRARY
) else if %kind% == SharedLib (
	set defines=%defines% -D CUSTOM_SHARED_LIBRARY -D CUSTOM_SYMBOLS_SHARE
) else if %kind% == ConsoleApp (
	set defines=%defines% -D CUSTOM_APPLICATION
) else if %kind% == WindowedApp (
	set defines=%defines% -D CUSTOM_APPLICATION
)

set compiler=%compiler% %defines%

rem //
rem // >> COMPILER: optimization
rem //
set optimization=
if %configuration% == Shipping (
	set optimization=%optimization% -Ox -Oi -GF -Gy
) else if %configuration% == Development (
	set optimization=%optimization% -Ox -Oi -GF -Gy
) else if %configuration% == Debug (
	set optimization=%optimization% -Od -Gs
)

rem Specify Floating-Point Behavior (fast)
set optimization=%optimization% -fp:fast
rem Specify Floating-Point Behavior (disabled exceptions)
set optimization=%optimization% -fp:except-

rem -Ob: Inline Function Expansion
rem -Og: Global Optimizations
rem -Oy: Frame-Pointer Omission
rem -Os: Favor Small Code
rem -Ot: Favor Fast Code
rem -Oi: Generate Intrinsic Functions
rem -GF: Eliminate Duplicate Strings
rem -Gs: Control Stack Checking Calls
rem -Gy: Enable Function-Level Linking
rem -GL: Whole Program Optimization

rem -Od: Disable (Debug)
rem -O1: Minimize Size: -Og -Os -Oy -Ob2 -Gs -GF -Gy
rem -O2: Maximize Speed: -Og -Oi -Ot -Oy -Ob2 -Gs -GF -Gy
rem -Ox: Full Optimization: -Og -Oi -Ot -Oy -Ob2

set compiler=%compiler% %optimization%

rem //
rem // >> COMPILER: debug, slow compile times
rem //
set debug=
if %configuration% == Development (
	set debug=%debug% -Zi -Zf -JMC -Zo
) else if %configuration% == Debug (
	set debug=%debug% -Zi -Zf -JMC
)

rem -Zo:  Enhance Optimized Debugging
rem -Z7:  Produces an .obj file containing full symbolic debugging information
rem -Zi:  Produces a program database
rem -ZI:  PDB, edit and continue
rem -Zf:  Faster PDB generation
rem -FC:  Full Path of Source Code File in Diagnostics
rem -JMC: Just My Code debugging

set compiler=%compiler% %debug%

rem //
rem // >> COMPILER: warnings
rem //
set warnings=-W4 -WX

rem nameless struct/union
set warnings=%warnings% -wd4201
rem local variable is initialized but not referenced
set warnings=%warnings% -wd4189
rem unreferenced formal parameter
set warnings=%warnings% -wd4100
rem C++ exception handling was used but -EHsc was not selected
rem set warnings=%warnings% -wd4530
rem 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
rem set warnings=%warnings% -wd4521

rem -W0:     disables all warnings
rem -W1:     displays level 1 (severe) warnings. -W1 is the default setting
rem -W2:     displays level 1 and level 2 (significant) warnings
rem -W3:     displays level 1, level 2 and level 3 (production quality) warnings
rem -W4:     displays level 1, level 2, and level 3 warnings, and all level 4 (informational) warnings that are not turned off by default
rem -Wall:   displays all warnings displayed by -W4 and all other warnings that -W4 does not include
rem -WX:     Treat Warnings as Errors

set compiler=%compiler% %warnings%

rem //
rem // >> COMPILER: build options
rem //
set build_options=-nologo
if %configuration% == Shipping (
	rem either bundle CRT (vcruntime140.dll, etc.)
	rem or build with [-MT]
	set build_options=%build_options% -MD
) else if %configuration% == Development (
	set build_options=%build_options% -MD
) else if %configuration% == Debug (
	set build_options=%build_options% -MDd
)

rem Enable Minimal Rebuild (disabled)
set build_options=%build_options% -Gm-
rem Enable Run-Time Type Information (disabled)
set build_options=%build_options% -GR-
rem Exception Handling Model: all (disabled)
set build_options=%build_options% -EHa-
rem Build with Multiple Processes
set build_options=%build_options% -MP
rem Calling Convention
set build_options=%build_options% -Gd
rem Remove unreferenced COMDAT
set build_options=%build_options% -Zc:inline
rem Force Conformance in for Loop Scope
set build_options=%build_options% -Zc:forScope
rem wchar_t Is Native Type
set build_options=%build_options% -Zc:wchar_t
rem Buffer Security Check
set build_options=%build_options% -GS

rem -nologo: Suppress Startup Banner
rem -c:   Compile Without Linking
rem -MT:  Use Run-Time Library (multithread, static); defines _MT
rem -MTd: Use Run-Time Library (multithread, static, debug); defines _MT, _DEBUG
rem -MD:  Use Run-Time Library (multithread, dynamic); defines _MT, _DLL
rem -MDd: Use Run-Time Library (multithread, dynamic, debug); defines _MT, _DLL, _DEBUG
rem -TP:  Specify Source File Type (C++ globally)
rem -Tp:  Specify Source File Type (C++ locally, per file)
rem -TC:  Specify Source File Type (C globally)
rem -Tc:  Specify Source File Type (C locally, per file)

set compiler=%compiler% %build_options%

rem //
rem // >> COMPILER: diagnostics
rem //
set diags=

rem -Bt+:          build throughput, verbose
rem -time+:        some linker switch, verbose
rem -d2cgsummary:  code generation summary
rem -d1reportTime: report compilation time

rem -diagnostics:classic: reports only the line number where the issue was found
rem -diagnostics:column   includes the column where the issue was found
rem -diagnostics:caret    includes the column where the issue was found and places a caret (^)

rem -errorReport:none:   Reports about internal compiler errors will not be collected or sent to Microsoft
rem -errorReport:prompt: Prompts you to send a report when you receive an internal compiler error. prompt is the default when an application is compiled in the development environment
rem -errorReport:queue:  Queues the error report. When you log in with administrator privileges, a window is displayed so that you can report any failures since the last time you were logged in (you will not be prompted to send reports for failures more than once every three days)
rem -errorReport:send:   Automatically sends reports of internal compiler errors to Microsoft if reporting is enabled by the Windows Error Reporting system settings

set compiler=%compiler% %diags%

rem //
rem // >> LINKER
rem //
set linker=-NOLOGO
set linker=%linker% "%intermediate_location%\*.obj"
set linker=%linker% %libraries%
if %kind% == SharedLib (
	set linker=%linker% -OUT:"%target_location%\%target_name%.dll" -DLL
) else if %kind% == ConsoleApp (
	set linker=%linker% -OUT:"%target_location%\%target_name%.exe"
) else if %kind% == WindowedApp (
	set linker=%linker% -OUT:"%target_location%\%target_name%.exe"
)
set linker=%linker% -WX
set linker=%linker% -MACHINE:%architecture_target:x=X%

rem set linker=%linker% -MANIFEST:EMBED
rem set linker=%linker% -MANIFESTUAC:"level='asInvoker' uiAccess='false'"
rem set linker=%linker% -TLBID:1
rem set linker=%linker% -DYNAMICBASE
rem set linker=%linker% -NXCOMPAT

rem Use Program Database
set linker=%linker% -PDB:"%target_location%\%target_name%.pdb"
rem Name Import Library
set linker=%linker% -IMPLIB:"%intermediate_location%\%target_name%.lib"

if %configuration% == Shipping (
	set linker=%linker% -INCREMENTAL:NO -OPT:REF -OPT:ICF -DEBUG:NONE
) else if %configuration% == Development (
	set linker=%linker% -INCREMENTAL:NO -OPT:REF -OPT:ICF -DEBUG:FASTLINK
) else if %configuration% == Debug (
	set linker=%linker% -INCREMENTAL:NO -OPT:NOREF -OPT:NOICF -DEBUG:FULL
)

rem -INCREMENTAL:NO: Link Incrementally (disabled)
rem -OPT:REF:        Optimizations (eliminates functions and data that are never referenced)
rem -OPT:ICF:        Optimizations (perform identical COMDAT folding)
rem -DEBUG:####:     Generate Debug Info (NONE|FULL|FASTLINK)
rem -LTCG:           Link-time Code Generation
rem                  > (NO|INCREMENTAL)
rem -MACHINE:####:   Specify Target Platform (ARM|EBC|X64|X86)
rem -MANIFEST:       Create Side-by-Side Assembly Manifest
rem                  > EMBED: option specifies that the linker should embed the manifest file in the image as a resource of type RT_MANIFEST
rem -MANIFESTUAC:    Embeds UAC information in manifest
rem                  > level:(asInvoker|highestAvailable|requireAdministrator)
rem                  > uiAccess:(false|true)
rem -TLBID:####:     Specify Resource ID for TypeLib
rem -DYNAMICBASE:    Use address space layout randomization
rem -NXCOMPAT:       Compatible with Data Execution Prevention
rem -IMPLIB:####:    Name Import Library
rem -VERBOSE:        Print progress messages
rem                  > (CLR|ICF|INCR|LIB|REF|SAFESEH|UNUSEDDELAYLOAD|UNUSEDLIBS)

rem //
rem // >> LINKER: subsystem
rem //

if %kind% == StaticLib (
	rem blank
) else if %kind% == SharedLib (
	rem use int __stdcall DllMain(...) { }
	rem set linker=%linker% -ENTRY:_DllMainCRTStartup
) else if %kind% == ConsoleApp (
	rem use int main(...) { }
	rem set linker=%linker% -ENTRY:mainCRTStartup
	rem set linker=%linker% -ENTRY:wmainCRTStartu
	if %architecture_target% == x64 (
		set linker=%linker% -SUBSYSTEM:CONSOLE,5.02
	) else if %architecture_target% == x86 (
		set linker=%linker% -SUBSYSTEM:CONSOLE,5.01
	) else (
		set linker=%linker% -SUBSYSTEM:CONSOLE
	)
) else if %kind% == WindowedApp (
	rem use int __stdcall WinMain(...) { }
	rem set linker=%linker% -ENTRY:WinMainCRTStartup
	rem set linker=%linker% -ENTRY:wWinMainCRTStartup
	if %architecture_target% == x64 (
		set linker=%linker% -SUBSYSTEM:WINDOWS,5.02
	) else if %architecture_target% == x86 (
		set linker=%linker% -SUBSYSTEM:WINDOWS,5.01
	) else (
		set linker=%linker% -SUBSYSTEM:WINDOWS
	)
)

rem //
rem // >> LIBRARY MANAGER
rem //
set lib_manager=-NOLOGO
set lib_manager=%lib_manager% "%intermediate_location%\*.obj"
set lib_manager=%lib_manager% -OUT:"%target_location%\%target_name%.lib"
set lib_manager=%lib_manager% -WX
set lib_manager=%lib_manager% -MACHINE:%architecture_target:x=X%

rem //
rem // >> CL
rem //

rem prepare folders, clean up, build
if not exist %target_location% mkdir %target_location%
if not exist %intermediate_location% mkdir %intermediate_location%

rem pushd %target_location%
rem popd

rem pushd %intermediate_location%
rem popd

rem pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
pushd "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
if %architecture_compiler% == %architecture_target% (
	call "vcvarsall.bat" %architecture_compiler%
) else (
	call "vcvarsall.bat" %architecture_compiler%_%architecture_target%
)
popd

rem -showIncludes: List Include Files
rem -E: Preprocess to stdout (with "> %target_name%.cpp" at the end)

echo ClCompile:
echo %compiler%
if %kind% == StaticLib (
	echo Lib:
	echo %lib_manager%
) else (
	echo Link:
	echo %linker%
)

echo ---- BUILD ---- %time%
cl "%compilation_unit%" -c %compiler%
if %kind% == StaticLib (
	lib %lib_manager%
) else (
	link %linker%
)
rem set compiler=%compiler% -Fe"%target_location%\%target_name%"
rem cl "%compilation_unit%" %compiler% -link %linker%
echo ---- DONE ---- %time%

if %target_name% == demo_game (
	xcopy /Q /E /Y /I "%target_location%\demo_game*" "%target_location%\..\platform_windows"
)
