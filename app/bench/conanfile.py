import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy

class VTXRendererBenchRecipe(ConanFile):
    name = "vtx_renderer_bench"
    version = "1.0"
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "vendor/*", "cmake/*", "data/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_io/1.0")
        self.requires("sdl/3.2.14")
        self.requires("imgui/1.91.8")        
        #self.requires("assimp/5.2.2") 
    
    def generate(self):
        tc = CMakeToolchain(self)
        dir_shaders = self.dependencies["vtx_renderer"].conf_info.get("user.myconf:dir_shaders")
        tc.cache_variables["DIR_SHADERS"] = dir_shaders
        tc.generate()
        
        copy(self, "*sdl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))      
            
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
        self.cpp_info.libs = ["vtx_renderer_bench"]
        self.cpp_info.includedirs = ["include", "vendor"]
