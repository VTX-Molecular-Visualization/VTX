import os
import shutil
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.cmake import CMakeDeps
from conan.tools.files import copy
from conan.tools.cmake import CMakeToolchain
from pathlib import Path

def copy_gromacs_stuff(p_conanFile: ConanFile):
    copy(p_conanFile, "*", os.path.join(p_conanFile.dependencies["gromacs"].package_folder, "external"), os.path.join(p_conanFile.build_folder, "external"))        
    copy(p_conanFile, "*", os.path.join(p_conanFile.dependencies["gromacs"].package_folder, "data", "tools","mdprep","gromacs","top"), os.path.join(p_conanFile.build_folder, "data", "tools", "mdprep", "gromacs", "top" ))   

class VTXToolMdprepRecipe(ConanFile):
    name = "vtx_tool_mdprep"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "test/*", "data/*", "asset/*"

     
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_ui_qt/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("re2/20240702")
        self.requires("gromacs/2026.0")
        self.requires("catch2/3.13.0")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["CPYTHON_VERSION_MAJOR"] = self.dependencies["vtx_python_binding"].conf_info.get("user.python_binding:cpython_version_major")
        tc.cache_variables["CPYTHON_VERSION_MINOR"] = self.dependencies["vtx_python_binding"].conf_info.get("user.python_binding:cpython_version_minor")
        tc.cache_variables["CPYTHON_VERSION_PATCH"] = self.dependencies["vtx_python_binding"].conf_info.get("user.python_binding:cpython_version_patch")
        tc.generate()
        copy(self, "*.dll", self.dependencies["vtx_ui_qt"].cpp_info.bindir, os.path.join(self.build_folder, self.cpp.build.libdirs[0]))
        
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
        
    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_mdprep"]
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_tool_mdprep_copy_files.cmake"])
        
