import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy

class VTXRendererRecipe(ConanFile):
    name = "vtx_renderer"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False]}
    default_options = {"shared": False, "fPIC": True, "test": False}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "vendor/*", "shaders/*", "cmake/*", "test/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("catch2/3.8.1")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def generate(self):
        copy(self, "*.cmake", self.source_folder, self.build_folder)
        
    def layout(self):
        cmake_layout(self)
        
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build() 
        if self.options.test == True:
            cmake.ctest(["--output-on-failure"])

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.cmake", self.build_folder, self.package_folder)

    def package_info(self):    
        self.cpp_info.libs = ["vtx_renderer"]      
        self.conf_info.define("user.myconf:dir_shaders", os.path.join(self.package_folder, "shaders"))
        self.cpp_info.set_property("cmake_build_modules", ["cmake/copy_shaders.cmake"])
        