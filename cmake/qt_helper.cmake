find_package(Qt6 COMPONENTS Core REQUIRED)

# get absolute path to qmake, then use it to find windeployqt executable

get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

function(win_deploy_qt target)

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

function (configure_qt src_folder)

	set(CMAKE_AUTOMOC ON PARENT_SCOPE)
	set(CMAKE_AUTORCC ON PARENT_SCOPE)
	set(CMAKE_AUTOUIC ON PARENT_SCOPE)
	set(CMAKE_AUTOUIC_SEARCH_PATHS "${src_folder}/qt/qt_form" PARENT_SCOPE)
	#set(CMAKE_AUTOGEN_TARGETS_FOLDER "autogen")
	set_property(GLOBAL PROPERTY USE_FOLDERS ON PARENT_SCOPE)
	set(CMAKE_INCLUDE_CURRENT_DIR ON PARENT_SCOPE)
	#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	
endfunction()

# Fill FORMS and QT_RESOURCES file lists
function (find_qt_files)

	# Get all QT forms.
	file(GLOB_RECURSE INTERNAL_FORMS *.ui)
	set(FORMS ${INTERNAL_FORMS} PARENT_SCOPE)
	foreach(SRC IN ITEMS ${INTERNAL_FORMS})
		# Create group to make visual studio filters.
		source_group("Forms" FILES "${SRC}")
	endforeach()

	# Get all QT Resources Files.
	file(GLOB_RECURSE INTERNAL_QT_RESOURCES *.qrc)
	set(QT_RESOURCES ${INTERNAL_QT_RESOURCES} PARENT_SCOPE)
	
	foreach(SRC IN ITEMS ${INTERNAL_QT_RESOURCES})
		# Create group to make visual studio filters.
		source_group("Resources" FILES "${SRC}")
	endforeach()
	
endfunction()


function (apply_qt_options target)

	set_property(TARGET ${target} PROPERTY AUTOUIC_OPTIONS "--no-autoconnection")
	add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
	
endfunction()