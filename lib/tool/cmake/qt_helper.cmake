function (configure_qt)

	set(CMAKE_AUTOMOC ON PARENT_SCOPE)
	set(CMAKE_AUTORCC ON PARENT_SCOPE)
	set(CMAKE_AUTOUIC ON PARENT_SCOPE)
	set(CMAKE_AUTOUIC_SEARCH_PATHS "asset/qt/forms" PARENT_SCOPE)
	#set(CMAKE_AUTOGEN_TARGETS_FOLDER "autogen")
	set_property(GLOBAL PROPERTY USE_FOLDERS ON PARENT_SCOPE)
	set(CMAKE_INCLUDE_CURRENT_DIR ON PARENT_SCOPE)
	#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	
endfunction()


function (apply_qt_options target)

	set_property(TARGET ${target} PROPERTY AUTOUIC_OPTIONS "--no-autoconnection")
	add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
	
endfunction()