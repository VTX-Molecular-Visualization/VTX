function(vtx_copy_file target source destination)
	add_custom_command(
		TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${source}"
			"${destination}"
		VERBATIM
	)
endfunction()

function(vtx_copy_directory target source destination)
	add_custom_command(
		TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
			"${source}"
			"${destination}"
		VERBATIM
	)
endfunction()
