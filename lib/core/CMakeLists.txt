message("Project library: VTX_CORE")

file(GLOB_RECURSE HEADERS core/*.hpp)
file(GLOB_RECURSE SOURCES core/*.cpp)

add_library(VTX_CORE STATIC ${HEADERS} ${SOURCES})

target_include_directories(VTX_CORE PRIVATE core/)
target_include_directories(VTX_CORE PUBLIC .)

target_link_libraries(VTX_CORE PRIVATE VTX_UTIL)