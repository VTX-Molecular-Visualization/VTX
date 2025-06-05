@ECHO OFF
FOR /d /r . %%d IN (build) DO @IF EXIST "%%d" rd /s /q "%%d"
PAUSE