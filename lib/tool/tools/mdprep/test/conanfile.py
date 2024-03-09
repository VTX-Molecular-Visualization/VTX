import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.files import copy

from pathlib import Path



class VTXRendererTestRecipe(ConanFile):
    name = "vtx_tool_mdprep_test"
    version = "1.0"    
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeToolchain", "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "cmake/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("qt/6.6.1", transitive_headers=True)
        self.requires("vtx_tool_mdprep/1.0")
        self.requires("catch2/3.4.0")        
        
    def generate(self):
        copy(self, "*", os.path.join(self.dependencies["gromacs"].package_folder, "external"), os.path.join(self.build_folder, "external"))        
        copy(self, "*", os.path.join(self.dependencies["gromacs"].package_folder, "data", "tools","mdprep","gromacs","top"), os.path.join(self.build_folder, "data", "tools", "mdprep", "gromacs", "top" ))    
    
    def layout(self):
        cmake_layout(self)

    def build(self):
        """
        vtx_gromacs_shared = os.path.join(self.dependencies["vtx_tool_mdprep"].folders.build_folder, "vtx_gromacs")
        ext = ".dll"
        if self.settings.os == "iOS" :
            ext = ".dylib"
        if self.settings.os == "Linux" :
            ext = ".so"
        vtx_gromacs_shared += ext
        """
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        #self.run("ctest --rerun-failed --output-on-failure") # TODO uncomment this when build is stable

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_mdprep_test"] 
        None