@ECHO OFF
cd /d "%~dp0\.."

NET SESSION >NUL 2>&1
IF ERRORLEVEL 1 (
	ECHO This script must be run as administrator.
	PAUSE
	EXIT /b 1
)

SET BUILD_TYPE=%~1
IF "%BUILD_TYPE%"=="" SET BUILD_TYPE=Debug
SET PRESET=conan-debug
IF /I "%BUILD_TYPE%"=="Release" SET PRESET=conan-release
SET BUILD_DIR=dev\build\%BUILD_TYPE%
SET TRACE_FILE=%CD%\%BUILD_DIR%\build_insights.etl

FOR /f "usebackq delims=" %%i IN (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) DO SET VS_DIR=%%i
CALL "%VS_DIR%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -no_logo

IF NOT EXIST "%BUILD_DIR%\CMakeCache.txt" (
	PUSHD dev
	CALL conan install . --build=missing -s build_type=%BUILD_TYPE%
	cmake --preset %PRESET%
	POPD
)

vcperf /start VTX
cmake --build %BUILD_DIR% --clean-first
vcperf /stop VTX "%TRACE_FILE%"

ECHO Trace: %TRACE_FILE%
PAUSE
