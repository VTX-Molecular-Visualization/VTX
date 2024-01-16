function(register_build_directory_copy dir)
	set(_REGISTER_DIR_COPY PARENT_SCOPE)
	set(_REGISTER_DIR_COPY "${_REGISTER_DIR_COPY}" CACHE INTERNAL "")
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_property "${_REGISTER_DIR_COPY_property};${dir}")
	#set(ENV{_REGISTER_DIR_COPY} "$ENV{_REGISTER_DIR_COPY};${dir}")
	list(APPEND _REGISTER_DIR_COPY ${dir})
	get_property(v GLOBAL PROPERTY _REGISTER_DIR_COPY_property)
	message("register_build_directory_copy -  input : <${dir}>\nEnv var : <$ENV{_REGISTER_DIR_COPY}>\nglobal var : <${_REGISTER_DIR_COPY}>\nproperty var : <${v}>")
	#write_file("./some_file.txt" "${dir}")
endfunction()

function(copy_directory_data target dest)
	set(num 0)
	message("copy_directory_data - Env var : <$ENV{_REGISTER_DIR_COPY}>")
	message("copy_directory_data - global var : <${_REGISTER_DIR_COPY}>")
	get_property(v GLOBAL PROPERTY _REGISTER_DIR_COPY_property)
	message("copy_directory_data - property var : <${v}>")

	set(hopefully_a_list $ENV{_REGISTER_DIR_COPY})
	list(SORT hopefully_a_list)

	#foreach(dir_it IN hopefully_a_list)
	#	add_custom_target("${target}_directory_copy${num}" ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different dir_it dest)
	#	set(num  (num + 1))
	#endforeach()
endfunction()