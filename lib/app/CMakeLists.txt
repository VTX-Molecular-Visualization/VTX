message("Project library: VTX_APP")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Create paths.
set(VTX_APP_SOURCE_PATH "lib/app/app")

# Libs URL and tags.
set(GIT_URL_CHEMFILES https://github.com/sacraiou/chemfiles.git)
set(GIT_TAG_CHEMFILES e6177423c015ac6ff1a0307b3c3a67c7850d0fe2)
set(GIT_URL_JSON https://github.com/nlohmann/json.git)
set(GIT_TAG_JSON v3.10.5) #3.10.5
set(GIT_URL_MAGIC_ENUM https://github.com/Neargye/magic_enum.git)
set(GIT_TAG_MAGIC_ENUM v0.8.0) #0.8.0

# Copy all sources.
message("Copying all sources...")

file(GLOB_RECURSE HEADERS app/*.hpp)
foreach(SRC IN ITEMS ${HEADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${VTX_APP_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Header${GROUP_PATH}" FILES "${SRC}")
endforeach()

file(GLOB_RECURSE SOURCES app/*.cpp)
foreach(SRC IN ITEMS ${SOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${VTX_APP_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Source${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all GLSL sources.
file(GLOB_RECURSE SHADERS old/*.vert old/*.frag old/*.geom old/*.tesc old/*.tese old/*.comp old/*.glsl)
foreach(SRC IN ITEMS ${SHADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Source" FILES "${SRC}")
endforeach()

# Download dependencies.
message("Downloading dependencies...")

include(FetchContent)
include(ExternalProject)

# Chemfiles.
FetchContent_Declare(
  chemfiles
  GIT_REPOSITORY ${GIT_URL_CHEMFILES}
  GIT_TAG ${GIT_TAG_CHEMFILES}
)
FetchContent_MakeAvailable(chemfiles)

# Json.
ExternalProject_Add(
    json
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_EXTERNAL_PROJECT}/nlohmann
    GIT_REPOSITORY ${GIT_URL_JSON}
	GIT_TAG ${GIT_TAG_JSON}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(json SOURCE_DIR)
set(JSON_INCLUDE_DIR ${SOURCE_DIR}/include/)

# Magic Enum.
ExternalProject_Add(
    magic_enum
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_EXTERNAL_PROJECT}/Neargye
    GIT_REPOSITORY ${GIT_URL_MAGIC_ENUM}
	GIT_TAG ${GIT_TAG_MAGIC_ENUM}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(magic_enum SOURCE_DIR)
set(MAGIC_ENUM_INCLUDE_DIR ${SOURCE_DIR}/include)

# Declare library with source files.
add_library(${VTX_APP_LIB} STATIC ${HEADERS} ${SOURCES} ${SHADERS})

# Includes local directories.
target_include_directories(${VTX_APP_LIB} PRIVATE app/)
target_include_directories(${VTX_APP_LIB} PUBLIC .)

# Includes external libraries directories.
target_include_directories(${VTX_APP_LIB} PUBLIC ${JSON_INCLUDE_DIR})
target_include_directories(${VTX_APP_LIB} PUBLIC ${MAGIC_ENUM_INCLUDE_DIR})
target_link_libraries(${VTX_APP_LIB} PUBLIC chemfiles)

# Includes VTX_Libs
target_include_directories(${VTX_APP_LIB} PUBLIC ${VTX_UTIL_INCLUDE_PATH})

target_link_libraries(${VTX_APP_LIB} PUBLIC ${VTX_UTIL_LIB})
# target_link_libraries(${VTX_APP_LIB} PRIVATE ${VTX_APP_CORE})
# target_link_libraries(${VTX_APP_LIB} PRIVATE ${VTX_APP_RENDER})
