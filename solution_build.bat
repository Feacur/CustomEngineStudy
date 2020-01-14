@echo off

chcp 1252
rem code page 1250: Windows Central Europe
rem code page 1251: Windows Cyrillic
rem code page 1252: Windows Western

set VSLANG=1033
rem msbuild locale 1033: English

rem read batch file arguments
set solution=%1
set architecture=%2
set configuration=%3

if [%architecture%] == [] (
	echo provide architecture [x64, x86]
	exit /b 0
)

if [%configuration%] == [] (
	echo provide kind [Debug, Development, Shipping]
	exit /b 0
)

rem clean batch file arguments
set solution=%solution:"=%
set architecture=%architecture:"=%
set configuration=%configuration:"=%

rem pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
pushd "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
call "vcvarsall.bat" %architecture%
popd

echo ---- BUILD ---- %time%
msbuild %solution% -m -p:Configuration=%configuration%
echo ---- DONE ---- %time%
