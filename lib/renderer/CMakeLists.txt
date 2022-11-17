message("Project library: VTX_RENDERER")

set(GIT_URL_GL3W https://github.com/skaslev/gl3w.git)
set(GIT_TAG_GL3W 5f8d7fd) # 24 March 2021
set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM bf71a83) #0.9.9.8

# GL3W
include(FetchContent)
FetchContent_Declare(
  gl3w
  GIT_REPOSITORY ${GIT_URL_GL3W}
  GIT_TAG        ${GIT_TAG_GL3W}
)
FetchContent_MakeAvailable(gl3w)
execute_process(COMMAND python ${gl3w_SOURCE_DIR}/gl3w_gen.py --root=${CMAKE_BINARY_DIR}/gl3w)

# Glm.
include(ExternalProject)
ExternalProject_Add(
    glm
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_EXTERNAL_PROJECT}/g-truc
    GIT_REPOSITORY ${GIT_URL_GLM}
	GIT_TAG ${GIT_TAG_GLM}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(glm SOURCE_DIR)
set(GLM_INCLUDE_DIR ${SOURCE_DIR})

file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(VTX_RENDERER STATIC ${HEADERS} ${SOURCES})

target_include_directories(VTX_RENDERER PUBLIC ${PATH_SRC})
target_include_directories(VTX_RENDERER PRIVATE ${CMAKE_BINARY_DIR}/gl3w/include)
target_include_directories(VTX_RENDERER PRIVATE ${CMAKE_BINARY_DIR}/gl3w/include/GL)
target_include_directories(VTX_RENDERER PUBLIC ${GLM_INCLUDE_DIR})

target_link_libraries(VTX_RENDERER PRIVATE VTX_UTIL)
