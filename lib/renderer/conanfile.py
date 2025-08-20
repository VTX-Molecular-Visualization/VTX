import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.files import copy

class VTXRendererRecipe(ConanFile):
    name = "vtx_renderer"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False], "cuda_arch": ["ANY"]}
    default_options = {"shared": False, "fPIC": True, "test": False, "cuda_arch": "native"}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "vendor/*", "shaders/*", "cmake/*", "test/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")
        self.requires("catch2/3.8.1")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["VTX_CUDA_ARCH"] = self.options.cuda_arch
        tc.generate()
        
    def layout(self):
        cmake_layout(self)
        self.cpp.source.includedirs = ["include/public"]
        
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build() 
        if self.options.test == True:
            cmake.ctest(["--output-on-failure"])

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):    
        self.cpp_info.libs = ["vtx_renderer"]      
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_renderer_copy_files.cmake", "cmake/vtx_link_cuda.cmake"])