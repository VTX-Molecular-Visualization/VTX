set(_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

# Create a dedicated stamped target so python scripts are synced once per runtime target.
function(_vtx_python_binding_add_python_script_target target source_dir)
	set(destination_dir "$<TARGET_FILE_DIR:${target}>/python_script")
	vtx_add_copy_directory_target(
		${target}
		"vtx_python_binding_copy_python_script"
		"${source_dir}"
		"${destination_dir}"
		"Syncing python_script for ${target}"
	)
endfunction()

# Bundle python scripts, embedded runtime files and Windows Python DLLs for a target.
function(vtx_python_binding_copy_runtime target)
	if(NOT IS_DIRECTORY "${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}/python_script")
		message(FATAL_ERROR "Unable to locate python_script under <${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}>.")
	endif()

	_vtx_python_binding_add_python_script_target(
		${target}
		"${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}/python_script"
	)

	if(NOT DEFINED VTX_PYTHON_BINDING_RUNTIME_ROOT OR VTX_PYTHON_BINDING_RUNTIME_ROOT STREQUAL "")
		message(FATAL_ERROR "VTX_PYTHON_BINDING_RUNTIME_ROOT must be defined before calling vtx_python_binding_copy_runtime().")
	endif()

	if(NOT IS_DIRECTORY "${VTX_PYTHON_BINDING_RUNTIME_ROOT}/external/python")
		message(FATAL_ERROR "Unable to locate prepared Python runtime under <${VTX_PYTHON_BINDING_RUNTIME_ROOT}>.")
	endif()

	vtx_copy_directory(
		${target}
		"${VTX_PYTHON_BINDING_RUNTIME_ROOT}/external/python"
		"$<TARGET_FILE_DIR:${target}>/external/python"
	)

	if(WIN32)
		file(GLOB _vtx_python_binding_runtime_dlls "${VTX_PYTHON_BINDING_RUNTIME_ROOT}/python*.dll")
		if(NOT _vtx_python_binding_runtime_dlls)
			message(FATAL_ERROR "Unable to locate CPython runtime under <${VTX_PYTHON_BINDING_RUNTIME_ROOT}>.")
		endif()
		vtx_copy_files(${target} "$<TARGET_FILE_DIR:${target}>" ${_vtx_python_binding_runtime_dlls})
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		if(NOT IS_DIRECTORY "${VTX_PYTHON_BINDING_RUNTIME_ROOT}/external/python/bin"
		   OR NOT IS_DIRECTORY "${VTX_PYTHON_BINDING_RUNTIME_ROOT}/external/python/lib")
			message(FATAL_ERROR "Unable to locate CPython runtime under <${VTX_PYTHON_BINDING_RUNTIME_ROOT}>.")
		endif()
	else()
		message(FATAL_ERROR "Unsupported platform for Python runtime copy.")
	endif()
endfunction()
