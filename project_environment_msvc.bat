@echo off

chcp 1252
rem code page 1250: Windows Central Europe
rem code page 1251: Windows Cyrillic
rem code page 1252: Windows Western

set VSLANG=1033
rem msbuild locale 1033: English

rem read batch file arguments
set architecture=%1
if [%architecture%] == [] (
	set architecture="x64"
)

rem clean batch file arguments
set architecture=%architecture:"=%

rem init
rem pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
pushd "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
call "vcvarsall.bat" %architecture%
popd
