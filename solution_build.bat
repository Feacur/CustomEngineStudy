@echo off

rem https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019

chcp 1252
rem code page 1250: Windows Central Europe
rem code page 1251: Windows Cyrillic
rem code page 1252: Windows Western

set VSLANG=1033
rem msbuild locale 1033: English

rem read batch file arguments
set solution=%1
set architecture_compiler=%2
set architecture_target=%3
set configuration=%4

if [%solution%] == [] (
	echo provide a solution
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

rem clean batch file arguments
set solution=%solution:"=%
set architecture_compiler=%architecture_compiler:"=%
set architecture_target=%architecture_target:"=%
set configuration=%configuration:"=%

rem pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
pushd "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
if %architecture_compiler% == %architecture_target% (
	call "vcvarsall.bat" %architecture_compiler%
) else (
	call "vcvarsall.bat" %architecture_compiler%_%architecture_target%
)
popd

echo ---- BUILD ---- %time%
msbuild %solution% -m -p:Configuration=%configuration%
echo ---- DONE ---- %time%
