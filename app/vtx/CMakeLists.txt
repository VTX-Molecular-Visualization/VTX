message("vtx")
cmake_minimum_required(VERSION 3.23)

project(vtx)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_ui_qt CONFIG REQUIRED)
if (VTX_TOOL_EXAMPLE)
	find_package(vtx_tool_example CONFIG REQUIRED)
endif()
if (VTX_TOOL_MDPREP)
	find_package(vtx_tool_mdprep CONFIG REQUIRED)
endif()

set(_VTX_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

# System libraries.
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
include(InstallRequiredSystemLibraries)
# Target.
install(TARGETS vtx DESTINATION .)
# Data from other libs.
vtx_install_target_directory(vtx "data")
vtx_install_target_directory(vtx "shaders")
vtx_install_target_directory(vtx "python_script")
vtx_install_target_directory(vtx "external")
# Data from VTX.
install(FILES README.md DESTINATION .)
install(FILES CHANGELOG.md DESTINATION .)
install(FILES license.txt DESTINATION .)
install(FILES asset/qt.conf DESTINATION .)
# Runtime DLLs.
#install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
vtx_qt_install_runtime(vtx)

# Infinite loop.
#install(DIRECTORY ${CMAKE_BINARY_DIR}/
#  DESTINATION .
#  FILES_MATCHING
#  PATTERN "*.dll"
#  PATTERN "install" EXCLUDE
#)

# Copy python and cuda dlls.
if(WIN32)
	vtx_install_target_files(vtx PATTERNS PATTERN "python*.dll")
endif()

include(CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
	find_package(CUDAToolkit REQUIRED)
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

include(CPack)
