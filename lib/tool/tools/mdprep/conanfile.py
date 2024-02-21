import os
import shutil
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.cmake import CMakeDeps
from conan.tools.files import copy
from conan.tools.cmake import CMakeToolchain
from pathlib import Path

class VTXToolMdprepRecipe(ConanFile):
    name = "vtx_tool_mdprep"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

     
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_ui/1.0")
        self.requires("vtx-gromacs/2024.0")
        self.requires("dylib/2.2.1")

    def generate(self):
        copy(self, "*.dll", self.dependencies["vtx_ui"].cpp_info.bindir, os.path.join(self.build_folder, self.cpp.build.libdirs[0]))
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        self.cpp.build.libdirs = self.cpp.build.libdirs
        self.cpp.source.includedirs = self.cpp.source.includedirs

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        
        gmx_bin_dir = os.path.join(self.dependencies["vtx-gromacs"].cpp_info.bindir, "bin")
        gmx_bin_dest = os.path.join(self.build_folder, self.cpp.build.libdirs[0], "external", "tools", "mdprep", "gromacs")
        for ext in ("*.exe", "*.dll", "*.a", "*.so", "*.dylib", "^[^.]$"): # No extension for executable in linux, right ? TODO !!!
            copy(self, ext, gmx_bin_dir, gmx_bin_dest)
        

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.dll", (self.dependencies["vtx-gromacs"].build_folder), os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_mdprep"]
        