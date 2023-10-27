import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.files import copy

class VTXRendererBenchRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    package_type = "application"
    
    generators = "CMakeToolchain", "CMakeDeps"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_io/1.0")
        self.requires("sdl/2.28.3")  
        self.requires("stb/cci.20230920")
        self.requires("imgui/1.89.9")
    
    def generate(self):
        copy(self, "*sdl2*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))
            
    def layout(self):
        cmake_layout(self)
        # Add vendor folder to the include path.
        self.cpp.source.includedirs = ["include", "vendor"]
        #self.cpp.package.includedirs = ["include", "vendor"]

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
