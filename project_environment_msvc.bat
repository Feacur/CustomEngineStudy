@echo off

rem https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019

chcp 65001
rem code page 65001: Unicode (UTF-8)
rem code page 1250:  ANSI Central European; Central European (Windows)
rem code page 1251:  ANSI Cyrillic; Cyrillic (Windows)
rem code page 1252:  ANSI Latin 1; Western European (Windows)

set VSLANG=1033
rem msbuild locale 1033: English

rem read batch file arguments
set architecture_compiler=%1
set architecture_target=%2

if [%architecture_compiler%] == [] (
	echo provide architecture_compiler [x64, x86]
	exit /b 0
)

if [%architecture_target%] == [] (
	echo provide architecture_target [x64, x86, arm, arm64]
	exit /b 0
)

rem clean batch file arguments
set architecture_compiler=%architecture_compiler:"=%
set architecture_target=%architecture_target:"=%

rem init
rem pushd "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build"
pushd "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build"
if %architecture_compiler% == %architecture_target% (
	call "vcvarsall.bat" %architecture_compiler%
) else (
	call "vcvarsall.bat" %architecture_compiler%_%architecture_target%
)
popd
