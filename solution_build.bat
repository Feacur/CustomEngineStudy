@echo off

chcp 1252
rem code page 1250: Windows Central Europe
rem code page 1251: Windows Cyrillic
rem code page 1252: Windows Western

set VSLANG=1033
rem msbuild locale 1033: English

rem read batch file arguments
set architecture=%1
set configuration=%2

if [%architecture%] == [] (
	set architecture="x64"
)

if [%configuration%] == [] (
	set configuration="Shipping"
)

rem clean batch file arguments
set architecture=%architecture:"=%
set configuration=%configuration:"=%

rem pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
pushd "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
call "vcvarsall.bat" %architecture%
popd

echo ---- BUILD ---- %time%
msbuild -m -p:Configuration=%configuration%
echo ---- DONE ---- %time%
