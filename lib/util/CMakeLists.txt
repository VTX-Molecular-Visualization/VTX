message("Project library: VTX_UTIL")

set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM 0.9.9.8)
set(GIT_URL_SPDLOG https://github.com/gabime/spdlog.git)
set(GIT_TAG_SPDLOG v1.11.0)

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

#Spdlog.
ExternalProject_Add(
    spdlog
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_EXTERNAL_PROJECT}/gabime
    GIT_REPOSITORY ${GIT_URL_SPDLOG}
	GIT_TAG ${GIT_TAG_SPDLOG}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(spdlog SOURCE_DIR)
set(SPDLOG_INCLUDE_DIR ${SOURCE_DIR})

set(VTX_UTIL_SOURCE_PATH "lib/util/util")

file(GLOB_RECURSE HEADERS util/*.hpp)
foreach(SRC IN ITEMS ${HEADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${VTX_UTIL_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Header${GROUP_PATH}" FILES "${SRC}")
endforeach()

file(GLOB_RECURSE SOURCES util/*.cpp)
foreach(SRC IN ITEMS ${SOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${VTX_UTIL_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Source${GROUP_PATH}" FILES "${SRC}")
endforeach()

add_library(${VTX_UTIL_LIB} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${VTX_UTIL_LIB} PRIVATE util/)
target_include_directories(${VTX_UTIL_LIB} PRIVATE .)
target_include_directories(${VTX_UTIL_LIB} PUBLIC ${GLM_INCLUDE_DIR})
target_include_directories(${VTX_UTIL_LIB} PUBLIC ${SPDLOG_INCLUDE_DIR}/include)