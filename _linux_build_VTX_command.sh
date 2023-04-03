#! /bin/bash

################################################
# Help                                         #
################################################
DisplayHelp()
{
    echo "Building a release for VTX"
    echo 
    echo "Syntax:"
    echo "buildVTX.sh [OPTIONS] VTX_FOLDER"
    echo ""
    echo "Options:"
    echo "  --clean         Clean previous build"
    echo "  -c; --copy      Copy build"
    echo "  -g; --git       Pull last version from git"
    echo "  -h; --help      Display help"
    echo "  -n; --name      Build directory name"
    echo "  --qt-path       Set the folder to use for qt library"
    echo "  --skip-make     Skip make"
    echo "  -z              Zip build"
    echo ""
}
################################################

################################################
# Clean                                        #
################################################
Clean()
{
    echo "Clean previous build"
    rm -rf build
    
}
################################################


################################################
# Pull from git                                #
################################################
PullFromGit()
{
    echo "Pull last dev version from git"
    git pull
    git checkout dev
}
################################################

################################################
# Make VTX                                     #
################################################
MakeVTX()
{
    echo "Make VTX"
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DDEFINE_PRODUCTION=ON -DCMAKE_PREFIX_PATH="$qtPath" .

    echo "Build VTX app"

    make -C build
}
################################################

################################################
# Move build file                           #
################################################
CopyBuild()
{
    echo "Copy build to " $copyBuildTarget
    
    if [ ! -d "$copyBuildDirectory" ]; then
        echo "Create directory" $copyBuildTarget
        mkdir -p -m 777 $copyBuildDirectory
    fi

    cp -r build/build $copyBuildTarget
}
################################################

################################################
# Clean build folder                           #
################################################
CleanBuildFolder()
{
    echo "Clean build folder"

    rm -r -f $buildName/libraries/representations
    rm -r -f $buildName/snapshots
    rm -r -f $buildName/logs
    rm -f $buildName/config.ini
    rm -f $buildName/setting.json
    rm -f $buildName/libraries/render_effects/Default
}
################################################

################################################
# Zip build                                    #
################################################
ZipBuild()
{
    echo "Archive build to $buildName.tar.gz"
    tar -czf "${buildName}.tar.gz" $copyBuildTarget
}
################################################

################################################
# Delete build copy                            #
################################################
DeleteBuildCopy()
{
    echo "Delete build copy"
    rm -rf $copyBuildDirectory
}
################################################


################################################
# Main Program                                 #
################################################

# Manage variables

copyBuild=false
copyBuildDirectory=.TMP
cleanMake=false
pullFromGit=false
zipBuild=false
makeVTX=true
vtxFolder=.
buildName=RELEASE
qtPath=.

# Manage options
while getopts ":hgzn:c:-:" option; do
    case $option in
        h) # Display Help
            DisplayHelp
            exit
            ;;
            
        g) # Set pull from git flag
            pullFromGit=true
            ;;
            
        c) # Copy Build
            copyBuild=true
            copyBuildDirectory=$OPTARG
            ;;
            
        n) # Set build name
            buildName=$OPTARG
            ;;
            
        z) # Zip build
            zipBuild=true
            ;;
            
        -)  # Manage long options
            case ${OPTARG} in
                clean) # Set clean flag
                    cleanMake=true
                    ;;
                    
                copy) # Copy Build
                    copyBuild=true
                    copyBuildDirectory=$OPTARG
                    ;;

                git) # Set pull from git flag
                    pullFromGit=true
                    ;;
                    
                name) # set build name
                    buildName="${!OPTIND}"
                    OPTIND=$(( $OPTIND + 1 ))
                    ;;
                    
                qt-path) # set qt path
                    qtPath="${!OPTIND}"
                    OPTIND=$(( $OPTIND + 1 ))
                    ;;
                    
                skip-make) # skip make
                    makeVTX=false
                    ;;

                *) # Invalid option
                    echo "Error: Invalid option"
                    exit
                    ;;
            esac
            ;;
    
        \?) # Invalid option
            echo "Error: Invalid option"
            exit
            ;;
    esac
done

vtxFolder="${!OPTIND}"
copyBuildTarget=$copyBuildDirectory/$buildName

echo "VTX Folder :" $vtxFolder
echo "QT Folder :" $qtPath

cd $vtxFolder

if "${cleanMake}"; then
    Clean
fi

if "${pullFromGit}"; then
    PullFromGit
fi

if "${makeVTX}" ; then
    MakeVTX
else
    echo "Skip make"
fi


if "${copyBuild}" || "${zipBuild}"; then
    CopyBuild
    CleanBuildFolder

    if "${zipBuild}"; then
        ZipBuild
    fi

    if ! "${copyBuild}" ; then
        DeleteBuildCopy
    fi
fi

echo "Build finished !"

exit
################################################
