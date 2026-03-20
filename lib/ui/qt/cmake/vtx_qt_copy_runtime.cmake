set(_VTX_QT_COPY_RUNTIME_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
find_program(_VTX_QT_PATCHELF_EXECUTABLE patchelf)

function(_vtx_qt_plugin_dirs out_var)
	set(${out_var}
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
		PARENT_SCOPE
	)
endfunction()

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
	if(NOT DEFINED VTX_QT_RUNTIME_ROOT OR VTX_QT_RUNTIME_ROOT STREQUAL "")
		message(FATAL_ERROR "VTX_QT_RUNTIME_ROOT must be defined before calling vtx_qt_copy_runtime().")
	endif()

	_vtx_qt_plugin_dirs(_vtx_qt_plugin_dirs)

	if(WIN32 AND (EXISTS "${VTX_QT_RUNTIME_ROOT}/Qt6Cored.dll" OR EXISTS "${VTX_QT_RUNTIME_ROOT}/Qt6Core.dll"))
		file(GLOB _vtx_qt_runtime_dlls
			"${VTX_QT_RUNTIME_ROOT}/Qt6Core*.dll"
			"${VTX_QT_RUNTIME_ROOT}/Qt6Gui*.dll"
			"${VTX_QT_RUNTIME_ROOT}/Qt6Widgets*.dll"
		)
		vtx_copy_files(${target} "$<TARGET_FILE_DIR:${target}>" ${_vtx_qt_runtime_dlls})

		foreach(_vtx_qt_plugin_dir IN LISTS _vtx_qt_plugin_dirs)
			if(EXISTS "${VTX_QT_RUNTIME_ROOT}/${_vtx_qt_plugin_dir}")
				vtx_copy_directory(
					${target}
					"${VTX_QT_RUNTIME_ROOT}/${_vtx_qt_plugin_dir}"
					"$<TARGET_FILE_DIR:${target}>/${_vtx_qt_plugin_dir}"
				)
			endif()
		endforeach()
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND EXISTS "${VTX_QT_RUNTIME_ROOT}/libQt6Core.so")
		file(GLOB _vtx_qt_runtime_sos
			"${VTX_QT_RUNTIME_ROOT}/libQt6*.so*"
		)
		vtx_copy_files(${target} "$<TARGET_FILE_DIR:${target}>" ${_vtx_qt_runtime_sos})

		foreach(_vtx_qt_plugin_dir IN LISTS _vtx_qt_plugin_dirs)
			if(EXISTS "${VTX_QT_RUNTIME_ROOT}/${_vtx_qt_plugin_dir}")
				vtx_copy_directory(
					${target}
					"${VTX_QT_RUNTIME_ROOT}/${_vtx_qt_plugin_dir}"
					"$<TARGET_FILE_DIR:${target}>/${_vtx_qt_plugin_dir}"
				)
				_vtx_qt_patch_linux_plugin_rpath(${target} "${_vtx_qt_plugin_dir}")
			endif()
		endforeach()
	else()
		message(FATAL_ERROR "Unable to locate a prepared Qt runtime under <${VTX_QT_RUNTIME_ROOT}>.")
	endif()
endfunction()

# Install the prepared Qt runtime copied next to a target.
function(vtx_qt_install_runtime target)
	_vtx_qt_plugin_dirs(_vtx_qt_plugin_dirs)

	if(WIN32)
		install(FILES
			"$<TARGET_FILE_DIR:${target}>/Qt6Core$<$<CONFIG:Debug>:d>.dll"
			"$<TARGET_FILE_DIR:${target}>/Qt6Gui$<$<CONFIG:Debug>:d>.dll"
			"$<TARGET_FILE_DIR:${target}>/Qt6Widgets$<$<CONFIG:Debug>:d>.dll"
			DESTINATION .
		)
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		vtx_install_target_files(
			${target}
			PATTERNS PATTERN "libQt6*.so*"
		)
	endif()

	foreach(_vtx_qt_plugin_dir IN LISTS _vtx_qt_plugin_dirs)
		vtx_install_target_directory(${target} "${_vtx_qt_plugin_dir}" OPTIONAL)
	endforeach()
endfunction()
