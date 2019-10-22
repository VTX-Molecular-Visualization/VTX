
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

SET(executables mmtf_demo traverse print_as_pdb tableexport read_and_write)

foreach(exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_compile_features(${exe} PRIVATE cxx_auto_type)
	if(WIN32)
		target_link_libraries(${exe} MMTFcpp ws2_32)
	else()
		target_link_libraries(${exe} MMTFcpp)
	endif()
endforeach(exe)

