message("Project library: VTX_APP")

file(GLOB_RECURSE HEADERS app/*.hpp)
file(GLOB_RECURSE SOURCES app/*.cpp)

add_library(VTX_APP STATIC ${HEADERS} ${SOURCES})

target_include_directories(VTX_APP PRIVATE app/)
target_include_directories(VTX_APP PUBLIC .)

target_link_libraries(VTX_APP PRIVATE VTX_UTIL)
target_link_libraries(VTX_APP PRIVATE VTX_CORE)
target_link_libraries(VTX_APP PRIVATE VTX_RENDER)