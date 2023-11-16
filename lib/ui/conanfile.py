import os
import glob
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy

from pathlib import Path

class VTXUiRecipe(ConanFile):
    name = "vtx_ui"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "asset/*"
        
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_app/1.0")
        self.requires("qt/6.6.0", transitive_headers=True)
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)      

    def generate(self):
        copy(self, "*.cmake", self.source_folder, self.build_folder)
        copy(self, "*.dll", self.dependencies["qt"].cpp_info.bindir, self.build_folder)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.cmake", self.build_folder, self.package_folder)
        copy(self, "*.dll", self.build_folder, self.package_folder)

    def package_info(self):
        self.cpp_info.libs = ["vtx_ui"]
        cmake_file = os.path.join("cmake", "qt_helper.cmake")
        self.cpp_info.set_property("cmake_build_modules", [cmake_file])
        if self.settings.os == "Windows":
            self.cpp_info.system_libs.append('d3d12')