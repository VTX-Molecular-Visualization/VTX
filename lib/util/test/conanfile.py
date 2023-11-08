from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake

class VTXUtilTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    package_type = "application"
    
    generators = "CMakeToolchain", "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("catch2/3.4.0")
        
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
