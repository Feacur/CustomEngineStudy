@echo off

rem https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019

chcp 65001
rem code page 65001: Unicode (UTF-8)
rem code page 1250:  ANSI Central European; Central European (Windows)
rem code page 1251:  ANSI Cyrillic; Cyrillic (Windows)
rem code page 1252:  ANSI Latin 1; Western European (Windows)

rem read batch file arguments
set target_location=%1

if [%target_location%] == [] (
	echo provide target location
	exit /b 0
)

rem clean batch file arguments
set target_location=%target_location:"=%

echo ---- PREPARE ASSETS: START ---- %time%

rem no messages
rem rd /Q ".\bin\%target_location%\sandbox\assets"
rem mklink /D ".\bin\%target_location%\sandbox\assets" "..\..\..\resources\assets"

rem no messages, always yes, all non-empty dirs, create dirs, only changed
set flags=/Q /Y /S /I /D
xcopy %flags% ".\custom_engine\assets" ".\bin\%target_location%\sandbox\assets"
xcopy %flags% ".\sandbox\assets" ".\bin\%target_location%\sandbox\assets"

echo ---- PREPARE ASSETS: DONE  ---- %time%
