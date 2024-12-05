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
        self.requires("catch2/3.7.0")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def generate(self):
        copy(self, "*.cmake", self.source_folder, self.build_folder)
        
    def layout(self):
        cmake_layout(self)
        
        self.cpp.build.components["vtx_renderer"].libdirs = self.cpp.build.libdirs
        #self.cpp.package.components["vtx_renderer"].includedirs = ["include", "vendor/glad/include"]
        
        self.cpp.build.components["vtx_renderer_no_opengl"].libdirs = self.cpp.build.libdirs
        #self.cpp.package.components["vtx_renderer_no_opengl"].includedirs = ["include"]
        
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
        self.cpp_info.components["vtx_renderer"].libs = ["vtx_renderer"]
        self.cpp_info.components["vtx_renderer"].set_property("cmake_target_name", "vtx_renderer::vtx_renderer")
        self.cpp_info.components["vtx_renderer"].requires = ["vtx_util::vtx_util"]
        #self.cpp_info.components["vtx_renderer"].includedirs = ["include", "vendor/glad/include"]

        self.cpp_info.components["vtx_renderer_no_opengl"].libs = ["vtx_renderer_no_opengl"]
        self.cpp_info.components["vtx_renderer_no_opengl"].set_property("cmake_target_name", "vtx_renderer::vtx_renderer_no_opengl")
        # self.cpp_info.components["vtx_renderer_no_opengl"].requires = ["vtx_util::vtx_util"]
        #self.cpp_info.components["vtx_renderer_no_opengl"].includedirs = ["include"]        
       
        self.cpp_info.components["vtx_renderer_test"].requires = ["catch2::catch2"]
         
        self.conf_info.define("user.myconf:dir_shaders", os.path.join(self.package_folder, "shaders"))
        self.cpp_info.set_property("cmake_build_modules", ["cmake/copy_shaders.cmake"])
        