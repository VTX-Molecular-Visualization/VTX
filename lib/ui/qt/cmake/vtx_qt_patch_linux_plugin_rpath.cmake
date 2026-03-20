# Invoked from a post-build command to rewrite Qt plugin RPATHs inside the bundle.
if(NOT DEFINED PATCHELF_EXECUTABLE OR NOT EXISTS "${PATCHELF_EXECUTABLE}")
	message(FATAL_ERROR "PATCHELF_EXECUTABLE is required to patch Qt plugin RPATHs")
endif()

if(NOT DEFINED PLUGIN_DIR OR NOT IS_DIRECTORY "${PLUGIN_DIR}")
	return()
endif()

if(NOT DEFINED RPATH)
	set(RPATH "$ORIGIN/..")
endif()

file(GLOB plugin_files "${PLUGIN_DIR}/*.so" "${PLUGIN_DIR}/*.so.*")

foreach(plugin_file IN LISTS plugin_files)
	execute_process(
		COMMAND "${PATCHELF_EXECUTABLE}" --set-rpath "${RPATH}" "${plugin_file}"
		COMMAND_ERROR_IS_FATAL ANY
	)
endforeach()
