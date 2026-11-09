@ECHO OFF
cd /d "%~dp0\.."

SET BUILD_TYPE=Debug
SET TEMPLATES_OPT=
SET TIMETRACE_OPT=
SET NOADMIN_OPT=
SET NOCPUSAMPLING_OPT=
FOR %%A IN (%*) DO (
	IF /I "%%A"=="Debug" SET BUILD_TYPE=Debug
	IF /I "%%A"=="Release" SET BUILD_TYPE=Release
	IF /I "%%A"=="templates" SET TEMPLATES_OPT=/templates
	IF /I "%%A"=="timetrace" SET TIMETRACE_OPT=/timetrace
	IF /I "%%A"=="noadmin" SET NOADMIN_OPT=/noadmin
	IF /I "%%A"=="nocpusampling" SET NOCPUSAMPLING_OPT=/nocpusampling
)

IF "%NOADMIN_OPT%"=="" (
	NET SESSION >NUL 2>&1
	IF ERRORLEVEL 1 (
		ECHO This script must be run as administrator, or with the noadmin option.
		PAUSE
		EXIT /b 1
	)
)

SET PRESET=conan-debug
IF /I "%BUILD_TYPE%"=="Release" SET PRESET=conan-release
SET BUILD_DIR=dev\build\%BUILD_TYPE%
SET TRACE_FILE=%CD%\%BUILD_DIR%\build_insights.etl
IF "%TIMETRACE_OPT%"=="/timetrace" SET TRACE_FILE=%CD%\%BUILD_DIR%\build_insights.json

FOR /f "usebackq delims=" %%i IN (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) DO SET VS_DIR=%%i
CALL "%VS_DIR%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -no_logo

IF NOT EXIST "%BUILD_DIR%\CMakeCache.txt" (
	PUSHD dev
	CALL conan install . --build=missing -s build_type=%BUILD_TYPE%
	cmake --preset %PRESET%
	POPD
)

vcperf /stopnoanalyze VTX "%TEMP%\vtx_build_insights_stale.etl" >NUL 2>&1
logman stop MSVC_BUILD_INSIGHTS_SESSION_VTX -ets >NUL 2>&1
vcperf /start %NOADMIN_OPT% %NOCPUSAMPLING_OPT% VTX
cmake --build %BUILD_DIR% --clean-first
vcperf /stop %TEMPLATES_OPT% VTX %TIMETRACE_OPT% "%TRACE_FILE%"

ECHO Trace: %TRACE_FILE%
PAUSE
