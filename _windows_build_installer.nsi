Unicode True

# Installer filename.
OutFile VTX_0.4.0_SelfExtractor.exe
 
# Installer directory.
InstallDir $EXEDIR\VTX_0.4.0

# Installer name.
Name "VTX v0.4.0"

# Meta tags.
VIProductVersion "0.4.0.0"
VIAddVersionKey "ProductName" "VTX"
VIAddVersionKey "CompanyName" "VTX Consortium"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2022"
VIAddVersionKey "FileDescription" "High performance molecular visualization software"
VIAddVersionKey "FileVersion" "0.4.0"

Section
 
# Output path.
SetOutPath $INSTDIR
 
# Add files.
File /r RELEASE\*
 
# Create shortcut.
CreateShortCut $INSTDIR\VTX.lnk $INSTDIR\bin\Release\VTX.exe

# define uninstaller name
WriteUninstaller $INSTDIR\uninstall.exe
 
SectionEnd
 
Section "Uninstall"

# Delete registry keys.
DeleteRegKey HKCU "SOFTWARE\VTX"

# Always delete uninstaller first.
Delete $INSTDIR\uninstall.exe
 
# Delete the directory.
RMDir /r $INSTDIR

SectionEnd