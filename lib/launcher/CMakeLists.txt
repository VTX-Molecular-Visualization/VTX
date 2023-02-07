message("Project library: VTX_LAUNCHER")

# Create output dir.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}})
	file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

file(GLOB_RECURSE HEADERS launcher/*.hpp)
file(GLOB_RECURSE SOURCES launcher/*.cpp)

add_executable(${VTX_LAUNCHER_LIB} ${HEADERS} ${SOURCES})

target_include_directories(${VTX_LAUNCHER_LIB} PRIVATE launcher/)

target_include_directories(${VTX_LAUNCHER_LIB} PRIVATE ${VTX_UTIL_INCLUDE_PATH})
target_include_directories(${VTX_LAUNCHER_LIB} PRIVATE ${OLD_VTX_INCLUDE_PATH})
target_include_directories(${VTX_LAUNCHER_LIB} PRIVATE ${VTX_UI_INCLUDE_PATH})

target_link_libraries(${VTX_LAUNCHER_LIB} PRIVATE ${VTX_UTIL_LIB})
target_link_libraries(${VTX_LAUNCHER_LIB} PRIVATE ${OLD_VTX_LIB})
target_link_libraries(${VTX_LAUNCHER_LIB} PRIVATE ${VTX_UI_LIB})


# Compiler options #2.
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "-fpermissive")
    target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "-w")
    #target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "-Wall")
    #target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "-Werror")
    #target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "-pedantic-errors")
elseif(MSVC)
	# General.
    target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "/W3")  # Warning level 3.
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "/WX")  # Warnings as errors.
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "/MP")  # Multicore compilation.
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "/sdl") # SDL check.
	
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "/utf-8")
	#target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "/DEBUG:FASTLINK") # https://devblogs.microsoft.com/cppblog/debugfastlink-for-vs2015-update-1/
	
	# Optimization.
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "$<$<CONFIG:Release>:/O2>")
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "$<$<CONFIG:Release>:/Ob2>")
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "$<$<CONFIG:Release>:/Ot>")
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "$<$<CONFIG:Release>:/Oi>")
	
	# Code Generation.
	target_compile_options(${VTX_LAUNCHER_LIB} PRIVATE "/fp:fast") # Floating Point Model.
	
	# Preprocessor definitions
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
endif()

message("Creating post build events...")

# Copy render effect presets to build directory.
add_custom_command(TARGET ${VTX_LAUNCHER_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/libraries/ ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../libraries)

# Copy Data.
add_custom_command(TARGET ${VTX_LAUNCHER_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/internal_data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../data)

# Readme, license and changelog.
add_custom_command(TARGET ${VTX_LAUNCHER_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../README.md)
add_custom_command(TARGET ${VTX_LAUNCHER_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/CHANGELOG.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../CHANGELOG.md)
add_custom_command(TARGET ${VTX_LAUNCHER_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/license.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../license.txt)

