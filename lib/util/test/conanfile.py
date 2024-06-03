from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake

class VTXUtilTestConan(ConanFile):
    name = "vtx_util_test"
    version = "1.0"
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeToolchain", "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "cmake/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("catch2/3.6.0")
        
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.ctest(["--output-on-failure"])
        
    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_util_test"] 
