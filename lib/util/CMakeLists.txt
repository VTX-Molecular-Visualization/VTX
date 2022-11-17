# Copy all sources.
message("Lib: VTX Util")

# Get all .hpp.
file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(VTX_UTIL ${HEADERS} ${SOURCES})
target_include_directories(VTX_UTIL PUBLIC ${PATH_SRC})