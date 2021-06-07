Unicode True

# Installer filename.
OutFile VTX_0.1.0_SelfExtractor.exe
 
# Install dir 
InstallDir $EXEDIR\VTX_0.1.0

Section
 
# Output path.
SetOutPath $INSTDIR
 
# Add files.
File /r build\*
 
# Create shortcut.
CreateShortCut $INSTDIR\VTX.lnk $INSTDIR\bin\Release\VTX.exe

# define uninstaller name
WriteUninstaller $INSTDIR\uninstall.exe
 
SectionEnd
 
Section "Uninstall"
 
# Always delete uninstaller first.
Delete $INSTDIR\uninstall.exe
 
# Delete the directory.
RMDir /r $INSTDIR

SectionEnd