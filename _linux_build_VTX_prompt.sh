#! /bin/bash
vtxVersion=0_4_RC1

qtStaticPath=/home/nicodak/Qt/6.2.4/Static/lib/cmake
qtDllPath=/home/nicodak/Qt/6.2.4/gcc_64/lib/cmake/

qtPath=$qtDllPath

echo "Enter Build type"
echo "0) Full build 1 (Clean - Updated - Packed - Copy)"
echo "1) Full build 2 (Clean - Updated - Packed)"

echo "2) Test build 1 (Updated - Copy)"
echo "3) Test build 2 (Copy)"

echo "4) Simple Make 1 (Updated)"
echo "5) Simple Make 2 (No options)"

echo "6) Just pack current build ! (No make)"

read buildType

case $buildType in
    0) # Full build 1 (Updated - Packed - Copy)
        ./_linux_build_VTX_command.sh --qt-path "$qtPath" -g -z -n "VTX_${vtxVersion}" -c . --clean .
        ;;
        
    1) # Full build 2 (Clean - Updated - Packed)
        ./_linux_build_VTX_command.sh --qt-path "$qtPath" -g -z -n "VTX_${vtxVersion}" --clean .
        ;;
        
    2) # Test build 1 (Updated - Copy)
        ./_linux_build_VTX_command.sh --qt-path "$qtPath" -g -n "VTX_${vtxVersion}" -c . .
        ;;
        
    3) # Test build 2 (Copy)
        ./_linux_build_VTX_command.sh --qt-path "$qtPath" -n "VTX_${vtxVersion}" -c . .
        ;;
        
    4) # Simple Make 1 (Updated)
        ./_linux_build_VTX_command.sh --qt-path "$qtPath" -g -n "VTX_${vtxVersion}" .
        ;;
        
    5) # Simple Make 2 (No options)
        ./_linux_build_VTX_command.sh --qt-path "$qtPath" -n "VTX_${vtxVersion}" .
        ;;
        
    6) # Just pack current build ! (No make)"
        ./_linux_build_VTX_command.sh --qt-path "$qtPath" -n  "VTX_${vtxVersion}" --skip-make -z .
        ;;
esac

exit
