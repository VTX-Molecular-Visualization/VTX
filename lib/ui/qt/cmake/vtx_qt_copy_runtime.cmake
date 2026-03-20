set(_VTX_QT_COPY_RUNTIME_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(_VTX_QT_COPY_RUNTIME_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
find_program(_VTX_QT_PATCHELF_EXECUTABLE patchelf)

# Patch Linux Qt plugins so they resolve bundled Qt libraries instead of system ones.
function(_vtx_qt_patch_linux_plugin_rpath target plugin_dir)
	if(NOT _VTX_QT_PATCHELF_EXECUTABLE)
		message(WARNING "patchelf not found, Qt plugin RPATH will not be patched for ${target}")
		return()
	endif()

	add_custom_command(
		TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
			-DPATCHELF_EXECUTABLE=${_VTX_QT_PATCHELF_EXECUTABLE}
			-DPLUGIN_DIR=$<TARGET_FILE_DIR:${target}>/${plugin_dir}
			-DRPATH=\$ORIGIN/..
			-P "${_VTX_QT_COPY_RUNTIME_CMAKE_DIR}/vtx_qt_patch_linux_plugin_rpath.cmake"
		VERBATIM
	)
endfunction()

# Bundle the Qt runtime required by a target from the first matching build/package root.
function(vtx_qt_copy_runtime target)
	set(_vtx_qt_runtime_roots)
	vtx_append_existing_realpath(_vtx_qt_runtime_roots "${_VTX_QT_COPY_RUNTIME_ROOT}")
	if(DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "")
		vtx_append_existing_realpath(_vtx_qt_runtime_roots "${_VTX_QT_COPY_RUNTIME_ROOT}/build/${CMAKE_BUILD_TYPE}")
	endif()
	vtx_append_existing_realpath(_vtx_qt_runtime_roots "${_VTX_QT_COPY_RUNTIME_ROOT}/build")
	vtx_append_existing_realpath(_vtx_qt_runtime_roots "${CMAKE_BINARY_DIR}")

	set(_vtx_qt_plugin_dirs
		imageformats
		platforms
		styles
		tls
		platformthemes
		xcbglintegrations
		egldeviceintegrations
		wayland-decoration-client
		wayland-graphics-integration-client
		wayland-shell-integration
	)

	foreach(_vtx_qt_runtime_root IN LISTS _vtx_qt_runtime_roots)
		if(WIN32 AND (EXISTS "${_vtx_qt_runtime_root}/Qt6Cored.dll" OR EXISTS "${_vtx_qt_runtime_root}/Qt6Core.dll"))
			file(GLOB _vtx_qt_runtime_dlls
				"${_vtx_qt_runtime_root}/Qt6Core*.dll"
				"${_vtx_qt_runtime_root}/Qt6Gui*.dll"
				"${_vtx_qt_runtime_root}/Qt6Widgets*.dll"
			)
			vtx_copy_files(${target} "$<TARGET_FILE_DIR:${target}>" ${_vtx_qt_runtime_dlls})

			foreach(_vtx_qt_plugin_dir IN LISTS _vtx_qt_plugin_dirs)
				if(EXISTS "${_vtx_qt_runtime_root}/${_vtx_qt_plugin_dir}")
					vtx_copy_directory(
						${target}
						"${_vtx_qt_runtime_root}/${_vtx_qt_plugin_dir}"
						"$<TARGET_FILE_DIR:${target}>/${_vtx_qt_plugin_dir}"
					)
				endif()
			endforeach()
		elseif(UNIX AND NOT APPLE AND EXISTS "${_vtx_qt_runtime_root}/libQt6Core.so")
			file(GLOB _vtx_qt_runtime_sos
				"${_vtx_qt_runtime_root}/libQt6*.so*"
			)
			vtx_copy_files(${target} "$<TARGET_FILE_DIR:${target}>" ${_vtx_qt_runtime_sos})

			foreach(_vtx_qt_plugin_dir IN LISTS _vtx_qt_plugin_dirs)
				if(EXISTS "${_vtx_qt_runtime_root}/${_vtx_qt_plugin_dir}")
					vtx_copy_directory(
						${target}
						"${_vtx_qt_runtime_root}/${_vtx_qt_plugin_dir}"
						"$<TARGET_FILE_DIR:${target}>/${_vtx_qt_plugin_dir}"
					)
					_vtx_qt_patch_linux_plugin_rpath(${target} "${_vtx_qt_plugin_dir}")
				endif()
			endforeach()
		endif()
	endforeach()
endfunction()
