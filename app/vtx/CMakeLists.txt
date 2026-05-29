message("vtx")
cmake_minimum_required(VERSION 3.23)

project(vtx)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
if (VTX_UI_QT)
	find_package(vtx_ui_qt CONFIG REQUIRED)
endif()
if (VTX_TOOL_EXAMPLE)
	find_package(vtx_tool_example CONFIG REQUIRED)
endif()
if (VTX_TOOL_MDPREP)
	find_package(vtx_tool_mdprep CONFIG REQUIRED)
endif()
if (VTX_TOOL_TOPOLOGY_EDITOR)
	find_package(vtx_tool_topology_editor CONFIG REQUIRED)
endif()

set(_VTX_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

# System libraries.
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
include(InstallRequiredSystemLibraries)
# Target.
install(TARGETS vtx DESTINATION .)
# Data from other libs.
if (VTX_TOOL_MDPREP)
	vtx_install_target_directory(vtx "data")
endif()
if (VTX_RENDERER)
	vtx_install_target_directory(vtx "shaders")
endif()
if (VTX_PYTHON_BINDING)
	vtx_install_target_directory(vtx "python_script")
	vtx_install_target_directory(vtx "external")
endif()
install(FILES license.txt DESTINATION .)
# Runtime DLLs.
#install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
if (VTX_UI_QT)
	vtx_qt_install_runtime(vtx)
endif()

# Infinite loop.
#install(DIRECTORY ${CMAKE_BINARY_DIR}/
#  DESTINATION .
#  FILES_MATCHING
#  PATTERN "*.dll"
#  PATTERN "install" EXCLUDE
#)

# Copy python libs.
if(WIN32 AND VTX_PYTHON_BINDING)
	vtx_install_target_files(vtx PATTERNS PATTERN "python*.dll")
endif()

# Copy cuda libs.
include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    find_package(CUDAToolkit REQUIRED)
    if(WIN32)
        set(CUDA_RUNTIME_PATTERN "${CUDAToolkit_BIN_DIR}/cudart*.dll")
    elseif(UNIX AND NOT APPLE)
        set(CUDA_RUNTIME_PATTERN "${CUDAToolkit_LIBRARY_DIR}/libcudart.so*")
    endif()

    if(CUDA_RUNTIME_PATTERN)
        file(GLOB CUDA_RUNTIME_FILES "${CUDA_RUNTIME_PATTERN}")

        if(CUDA_RUNTIME_FILES)
            install(FILES ${CUDA_RUNTIME_FILES} DESTINATION .)
        else()
            message(WARNING "No CUDA runtime found for pattern: ${CUDA_RUNTIME_PATTERN}")
        endif()
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

#include(CPack)
