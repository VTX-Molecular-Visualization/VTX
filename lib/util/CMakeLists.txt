# Copy all sources.
message("Lib: VTX Util")

# Get all .hpp.
file(GLOB_RECURSE HEADERS src/*.hpp)
add_library(VTX_UTIL ${HEADERS})
target_include_directories(VTX_UTIL PUBLIC ${PATH_SRC})
set_target_properties(VTX_UTIL PROPERTIES LINKER_LANGUAGE CXX)