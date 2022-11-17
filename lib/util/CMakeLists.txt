message("Project library: VTX_UTIL")

set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM bf71a83) #0.9.9.8

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

file(GLOB_RECURSE HEADERS util/*.hpp)
file(GLOB_RECURSE SOURCES util/*.cpp)

add_library(VTX_UTIL STATIC ${HEADERS} ${SOURCES})

target_include_directories(VTX_UTIL PRIVATE util/)
target_include_directories(VTX_UTIL PUBLIC .)
target_include_directories(VTX_UTIL PUBLIC ${GLM_INCLUDE_DIR})