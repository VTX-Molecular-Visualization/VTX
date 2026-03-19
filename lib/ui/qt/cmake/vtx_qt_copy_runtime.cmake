set(_VTX_QT_COPY_RUNTIME_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(_vtx_qt_append_runtime_root roots_var candidate)
	if(EXISTS "${candidate}")
		list(APPEND ${roots_var} "${candidate}")
		set(${roots_var} "${${roots_var}}" PARENT_SCOPE)
	endif()
endfunction()

function(_vtx_qt_copy_runtime_file target source destination)
	add_custom_command(
		TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${source}"
			"${destination}"
		VERBATIM
	)
endfunction()

function(_vtx_qt_copy_runtime_directory target source destination)
	add_custom_command(
		TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
			"${source}"
			"${destination}"
		VERBATIM
	)
endfunction()

function(vtx_qt_copy_runtime target)
	if(NOT WIN32)
		return()
	endif()

	set(_vtx_qt_runtime_roots)
	_vtx_qt_append_runtime_root(_vtx_qt_runtime_roots "${_VTX_QT_COPY_RUNTIME_ROOT}")
	if(DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "")
		_vtx_qt_append_runtime_root(_vtx_qt_runtime_roots "${_VTX_QT_COPY_RUNTIME_ROOT}/build/${CMAKE_BUILD_TYPE}")
	endif()
	_vtx_qt_append_runtime_root(_vtx_qt_runtime_roots "${_VTX_QT_COPY_RUNTIME_ROOT}/build")
	_vtx_qt_append_runtime_root(_vtx_qt_runtime_roots "${CMAKE_BINARY_DIR}")

	foreach(_vtx_qt_runtime_root IN LISTS _vtx_qt_runtime_roots)
		if(EXISTS "${_vtx_qt_runtime_root}/Qt6Cored.dll" OR EXISTS "${_vtx_qt_runtime_root}/Qt6Core.dll")
			file(GLOB _vtx_qt_runtime_dlls
				"${_vtx_qt_runtime_root}/Qt6Core*.dll"
				"${_vtx_qt_runtime_root}/Qt6Gui*.dll"
				"${_vtx_qt_runtime_root}/Qt6Widgets*.dll"
			)
			foreach(_vtx_qt_runtime_dll IN LISTS _vtx_qt_runtime_dlls)
				_vtx_qt_copy_runtime_file(${target} "${_vtx_qt_runtime_dll}" "$<TARGET_FILE_DIR:${target}>")
			endforeach()

			foreach(_vtx_qt_plugin_dir IN ITEMS imageformats platforms styles tls)
				if(EXISTS "${_vtx_qt_runtime_root}/${_vtx_qt_plugin_dir}")
					_vtx_qt_copy_runtime_directory(
						${target}
						"${_vtx_qt_runtime_root}/${_vtx_qt_plugin_dir}"
						"$<TARGET_FILE_DIR:${target}>/${_vtx_qt_plugin_dir}"
					)
				endif()
			endforeach()
		endif()
	endforeach()
endfunction()
