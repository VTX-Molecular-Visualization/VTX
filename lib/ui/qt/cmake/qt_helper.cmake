function( configure_qt )
	set( CMAKE_AUTOMOC ON PARENT_SCOPE )
endfunction()

function( add_resources p_target p_path p_resource_name)
	file(GLOB_RECURSE QT_RESOURCES ${p_path}/*)
	qt_add_resources( ${p_target} ${p_resource_name} BASE ${p_path} FILES ${QT_RESOURCES} )
endfunction()