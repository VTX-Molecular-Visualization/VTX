set(DIR_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

message("vtx")
cmake_minimum_required(VERSION 3.23)

project(vtx)

find_package(glm  REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(chemfiles CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(stb CONFIG CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)

include(CTest)
include(Catch)

# Util.
include("${DIR_LIBS}/util/cmake/library.cmake")

# Util test.
include("${DIR_LIBS}/util/test/cmake/library.cmake")

vtx_clear_registered_copies()

# Core.
include("${DIR_LIBS}/core/cmake/library.cmake")

# IO.
include("${DIR_LIBS}/io/cmake/library.cmake")

# IO test.
include("${DIR_LIBS}/io/test/cmake/library.cmake")

vtx_clear_registered_copies()

# Renderer.
include("${DIR_LIBS}/renderer/cmake/library.cmake")

# Renderer test.
include("${DIR_LIBS}/renderer/test/cmake/library.cmake")

# Renderer bench.
include("${DIR_LIBS}/renderer/bench/cmake/library.cmake")

vtx_clear_registered_copies()

# App.
include("${DIR_LIBS}/app/cmake/library.cmake")

# App test.
include("${DIR_LIBS}/app/test/cmake/library.cmake")

vtx_clear_registered_copies()

# Python binding.
include("${DIR_LIBS}/python_binding/cmake/library.cmake")

# Python binding test.
include("${DIR_LIBS}/python_binding/test/cmake/library.cmake")

vtx_clear_registered_copies()

# UI.
include("${DIR_LIBS}/ui/cmake/library.cmake")

# UI test.
include("${DIR_LIBS}/ui/test/cmake/library.cmake")

vtx_clear_registered_copies()

# MdPrep.
include("${DIR_LIBS}/tool/tools/mdprep/cmake/library.cmake")

# MdPrep test.
include("${DIR_LIBS}/tool/tools/mdprep/test/cmake/library.cmake")

