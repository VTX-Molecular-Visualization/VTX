@ECHO OFF
cd /d "%~dp0\.."
FOR /d /r . %%d IN (build) DO @IF EXIST "%%d" rd /s /q "%%d"
PAUSE
