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
if (VTX_TOOL_EXAMPLE)
	find_package(vtx_tool_example CONFIG REQUIRED)
endif()
if (VTX_TOOL_MDPREP)
	find_package(vtx_tool_mdprep CONFIG REQUIRED)
endif()

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)

set(_VTX_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

#set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)

install(TARGETS vtx DESTINATION .)
# Data from other libs.
install(DIRECTORY $<TARGET_FILE_DIR:vtx>/shaders DESTINATION .)
install(DIRECTORY $<TARGET_FILE_DIR:vtx>/python_script DESTINATION .)
install(DIRECTORY $<TARGET_FILE_DIR:vtx>/external DESTINATION .)
# Data from VTX.
install(FILES README.md DESTINATION .)
install(FILES CHANGELOG.md DESTINATION .)
install(FILES license.txt DESTINATION .)
# Runtime DLLs.
#install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
# DLLS from build dir.
if(WIN32)
	install(FILES
		$<TARGET_FILE_DIR:vtx>/Qt6Core$<$<CONFIG:Debug>:d>.dll
		$<TARGET_FILE_DIR:vtx>/Qt6Gui$<$<CONFIG:Debug>:d>.dll
		$<TARGET_FILE_DIR:vtx>/Qt6Widgets$<$<CONFIG:Debug>:d>.dll
		DESTINATION .
	)
	install(DIRECTORY $<TARGET_FILE_DIR:vtx>/platforms DESTINATION .)
	install(DIRECTORY $<TARGET_FILE_DIR:vtx>/imageformats DESTINATION .)
	install(DIRECTORY $<TARGET_FILE_DIR:vtx>/tls DESTINATION .)
	install(DIRECTORY $<TARGET_FILE_DIR:vtx>/styles DESTINATION .)
endif()

# Infinite loop.
#install(DIRECTORY ${CMAKE_BINARY_DIR}/
#  DESTINATION .
#  FILES_MATCHING
#  PATTERN "*.dll"
#  PATTERN "install" EXCLUDE
#)

# Copy python and cuda dlls.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
if(Python3_EXECUTABLE)
  get_filename_component(PYTHON_DIR "${Python3_EXECUTABLE}" DIRECTORY)
  file(GLOB PY_DLLS "${PYTHON_DIR}/python3*.dll")
	if(PY_DLLS)
		install(FILES ${PY_DLLS} DESTINATION .)
	else()
		message(WARNING "No Python DLL found in: ${PYTHON_DIR}")
	endif()
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