import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.files import copy

class VTXRendererTestRecipe(ConanFile):
    name = "vtx_tool_mdprep_test"
    version = "1.0"    
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeToolchain", "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_tool_mdprep/1.0")
        self.requires("catch2/3.4.0")        
        
    def generate(self):
        copy(self, "*.dll", self.dependencies["vtx_tool_mdprep"].cpp_info.bindir, os.path.join(self.build_folder, self.cpp.build.libdirs[0]))
        copy(
            self
            , "*"
            , os.path.join(self.dependencies["vtx_tool_mdprep"].cpp_info.bindir, "data")
            , os.path.join(self.build_folder, "data")
        )
    
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        #self.run("ctest --rerun-failed --output-on-failure") # TODO uncomment this when build is stable

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.dll", self.build_folder, os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_mdprep_test"] 