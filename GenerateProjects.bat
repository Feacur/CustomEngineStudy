@echo off

chcp 65001
rem code page 65001: Unicode (UTF-8)
rem code page 1250: ANSI Central European; Central European (Windows)
rem code page 1251: ANSI Cyrillic; Cyrillic (Windows)
rem code page 1252: ANSI Latin 1; Western European (Windows)

rem call vendor\premake\premake5.exe --file=CustomEngineStudy.lua vs2017
call vendor\premake\premake5.exe --file=CustomEngineStudy.lua vs2019
pause
