@echo off
setlocal

for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS=%%i"
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" >nul

cd /d "%~dp0"
for %%L in (OFF WARN) do call :bench %%L

echo.
echo clang_tidy=OFF  : %RES_OFF% s
echo clang_tidy=WARN : %RES_WARN% s
exit /b

:bench
if exist build rmdir /s /q build
set "T0=%TIME%"
call conan install . --build=missing -s build_type=Release -o "vtx/*:clang_tidy=%1"
cmake --preset conan-release
cmake --build build/Release
set "T1=%TIME%"
call :seconds "%T0%" S0
call :seconds "%T1%" S1
set /a "RES_%1=(S1-S0+86400) %% 86400"
exit /b

:seconds
set "t=%~1"
set "t=%t: =0%"
for /f "tokens=1-3 delims=:.," %%a in ("%t%") do set /a "%2=(1%%a %% 100)*3600 + (1%%b %% 100)*60 + (1%%c %% 100)"
exit /b
