message("Project library: VTX_RENDERER")

set(GIT_URL_GL3W https://github.com/skaslev/gl3w.git)
set(GIT_TAG_GL3W 5f8d7fd) # 24 March 2021

# GL3W
include(FetchContent)
FetchContent_Declare(
  gl3w
  GIT_REPOSITORY ${GIT_URL_GL3W}
  GIT_TAG        ${GIT_TAG_GL3W}
)
FetchContent_MakeAvailable(gl3w)
execute_process(COMMAND python ${gl3w_SOURCE_DIR}/gl3w_gen.py --root=${CMAKE_BINARY_DIR}/gl3w)

file(GLOB_RECURSE HEADERS renderer/*.hpp)
file(GLOB_RECURSE SOURCES renderer/*.cpp)

add_library(VTX_RENDERER STATIC ${HEADERS} ${SOURCES})

target_include_directories(VTX_RENDERER PRIVATE renderer/)
target_include_directories(VTX_RENDERER PUBLIC .)
target_include_directories(VTX_RENDERER PRIVATE ${CMAKE_BINARY_DIR}/gl3w/include)
target_include_directories(VTX_RENDERER PRIVATE ${CMAKE_BINARY_DIR}/gl3w/include/GL)

target_link_libraries(VTX_RENDERER PRIVATE VTX_UTIL)
