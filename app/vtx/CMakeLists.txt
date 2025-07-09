message("vtx")
cmake_minimum_required(VERSION 3.23)

project(vtx)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_ui_qt CONFIG REQUIRED)
find_package(vtx_python_binding CONFIG REQUIRED)
find_package(vtx_tool_example CONFIG REQUIRED)
find_package(vtx_tool_mdprep CONFIG REQUIRED)

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)

set(_VTX_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)
  
install(TARGETS vtx DESTINATION .)
# Data from other libs.
install(DIRECTORY ${CMAKE_BINARY_DIR}/shaders DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/python_script DESTINATION .)
# Data from VTX.
install(DIRECTORY libraries DESTINATION .)
install(DIRECTORY data DESTINATION .)
install(DIRECTORY internal_data DESTINATION .)
install(FILES README.md DESTINATION .)
install(FILES CHANGELOG.md DESTINATION .)
install(FILES license.txt DESTINATION .)
# Runtime DLLs.
install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
# DLLS from build dir.
file(GLOB QT_DLLS ${CMAKE_BINARY_DIR}/*.dll)
install(FILES ${QT_DLLS} DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/platforms DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/imageformats DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/tls DESTINATION .)

# Infinite loop.
#install(DIRECTORY ${CMAKE_BINARY_DIR}/
#  DESTINATION .
#  FILES_MATCHING
#  PATTERN "*.dll"
#  PATTERN "install" EXCLUDE
#)

# Copy python and cuda dlls.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(CUDAToolkit REQUIRED)

if(Python3_EXECUTABLE)
  get_filename_component(PYTHON_DIR "${Python3_EXECUTABLE}" DIRECTORY)
  file(GLOB PY_DLLS "${PYTHON_DIR}/python3*.dll")
	if(PY_DLLS)
		install(FILES ${PY_DLLS} DESTINATION .)
	else()
		message(WARNING "No Python DLL found in: ${PYTHON_DIR}")
	endif()
endif()

if (CMAKE_CUDA_COMPILER)
	file(GLOB CUDA_DLLS "${CUDAToolkit_BIN_DIR}/cudart*.dll")
	if(CUDA_DLLS)
		install(FILES ${CUDA_DLLS} DESTINATION .)
	else()
		message(WARNING "No Cuda DLL found in: ${CUDAToolkit_BIN_DIR}")
	endif()
endif()

# Include a lot of files from system32.
#install(CODE [[
#  file(GET_RUNTIME_DEPENDENCIES
#    EXECUTABLES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/vtx.exe"
#    RESOLVED_DEPENDENCIES_VAR resolved_deps
#    UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
#    PRE_INCLUDE_REGEXES ".*\\.[Dd][Ll][Ll]$"
#	#PRE_EXCLUDE_REGEXES "$ENV{WINDIR}"
#    DIRECTORIES ${CMAKE_BINARY_DIR} ${PYTHON_DIR} ${CUDAToolkit_BIN_DIR}
#  )
#  foreach(dep IN LISTS resolved_deps)
#    file(INSTALL DESTINATION "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}" TYPE SHARED_LIBRARY FILES "${dep}")
#  endforeach()
#
#  if (unresolved_deps)
#    message(WARNING "Certaines dépendances n'ont pas pu être résolues : ${unresolved_deps}")
#  endif()
#]])

set(CPACK_PACKAGE_NAME "VTX")
set(CPACK_PACKAGE_VENDOR "VTX dev team")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_EXECUTABLES "vtx" "VTX" )
set(CPACK_NSIS_MENU_LINKS "vtx.exe" "VTX")
  
set(CPACK_NSIS_INSTALL_ROOT "$LOCALAPPDATA")
# Create deskktop shortcut.
#set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "CreateShortCut '$DESKTOP/VTX.lnk' '$INSTDIR/vtx.exe' '' '' 'open' 0")


include(CPack)