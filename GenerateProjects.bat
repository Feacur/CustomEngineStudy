@echo off

chcp 65001
rem code page 65001: Unicode (UTF-8)
rem code page 1250: ANSI Central European; Central European (Windows)
rem code page 1251: ANSI Cyrillic; Cyrillic (Windows)
rem code page 1252: ANSI Latin 1; Western European (Windows)

rem read batch file arguments
set action=%1

if [%action%] == [] (
	echo provide an action
	exit /b 0
)

rem clean batch file arguments
set action=%action:"=%

call vendor\premake\premake5.exe --file=CustomEngineStudy.lua %action%
