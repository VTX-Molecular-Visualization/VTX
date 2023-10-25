from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake


class VTXUtilTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeToolchain", "CMakeDeps"
    requires = "vtx_util/1.0", "catch2/3.4.0"
        
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
