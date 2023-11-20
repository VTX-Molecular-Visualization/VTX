from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake

class VTXRendererTestRecipe(ConanFile):
    name = "vtx_renderer_test"
    version = "1.0"    
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeToolchain", "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("catch2/3.4.0")        
        
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        self.run("ctest --rerun-failed --output-on-failure")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_renderer_test"] 