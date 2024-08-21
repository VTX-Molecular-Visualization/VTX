function( configure_qt )
	set( CMAKE_AUTOMOC ON PARENT_SCOPE )
endfunction()

function( add_resources p_target p_path )
	file(GLOB_RECURSE QT_RESOURCES ${p_path}/*)
	qt_add_resources( ${p_target} vtx_qt_resources_ui BASE ${p_path} FILES ${QT_RESOURCES} )
endfunction()