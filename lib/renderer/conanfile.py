import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class VTXRendererRecipe(ConanFile):
    name = "vtx_renderer"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "shaders/*", "vendor/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)       
        self.cpp.source.includedirs = ["include", "vendor"]        

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_renderer"]        
        self.cpp_info.includedirs = ["include", "vendor"]
        self.conf_info.define("user.myconf:dir_shaders", os.path.join(self.package_folder, "shaders"))
        