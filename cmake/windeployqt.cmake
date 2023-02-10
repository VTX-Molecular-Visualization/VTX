find_package(Qt6 COMPONENTS Core REQUIRED)

# get absolute path to qmake, then use it to find windeployqt executable

get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

function(windeployqt target)

    # POST_BUILD step
    # - after build, we have a bin/lib for analyzing qt dependencies
    # - we run windeployqt on target and deploy Qt libs
	
	if (MSVC)
		string(APPEND CONFIG_FLAG
		"$<IF:$<CONFIG:Debug>,"
			"--debug,"
			"--release"
		">")
	else()
		string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
		if (BUILD_TYPE STREQUAL debug)
			string(APPEND CONFIG_FLAG "--" BUILD_TYPE )
		else()
			string(APPEND CONFIG_FLAG "--release" )
		endif()
	endif()
    
	add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${_qt_bin_dir}/windeployqt.exe"
				--verbose 2
				"${CONFIG_FLAG}"
				--no-translations
				--no-opengl-sw
				--no-system-d3d-compiler
				--libdir $<TARGET_FILE_DIR:${target}>
				--plugindir $<TARGET_FILE_DIR:${target}>
                \"$<TARGET_FILE:${target}>\"
        COMMENT "Deploying Qt libraries using windeployqt for compilation target '${target}' ..."
    )

endfunction()