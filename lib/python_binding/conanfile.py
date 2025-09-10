import os
import glob
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conan.tools.env import VirtualBuildEnv, Environment
from conan.tools.files import copy
from pathlib import Path

class VTXPythonBindingRecipe(ConanFile):    
    name = "vtx_python_binding"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False]}
    default_options = {"shared": False, "fPIC": True, "test": False}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "module/*", "include/*", "cmake/*", "python_script/*", "test/*"
    
    _former_LD_LIBRARY_PATH = None
    
    def _generated_cmake_prefix(self):
        return "pybind11-"
        
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")  
        self.requires("vtx_io/1.0")
        self.requires("pybind11/2.13.6", transitive_headers=True)
        self.requires("catch2/3.8.1")
        self.requires("cpython/3.9.19") # v >= 3.10 not working with msvc compiler so far
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

        self.options["cpython"].with_gdbm = False # Doesn't work on windows. I'm not sure what it does.
        if self.settings.os == "Windows":
            self.options["cpython"].shared = True # False by default. If set to False, DLLs will be missing.
        
            
    def generate(self):
        tc = CMakeToolchain(self)
        for r, d in self.dependencies.items(): 
            self.output.info(f"Requirement {r}")
            self.output.info(f"Is test {r.is_test} is override {r.override}")
        tc.generate()      
        
        if self.settings.os == "Windows":
            copy(self, "*", os.path.join(self.dependencies["cpython"].package_folder,"bin"), os.path.join(self.build_folder, "external","python"))        
        else:
            for subdir in ("bin","lib","include"):
                copy(self, "*", os.path.join(self.dependencies["cpython"].package_folder, subdir), os.path.join(self.build_folder, "external","python",subdir)) 
        
    
    def layout(self):
        cmake_layout(self)

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
        self.cpp_info.libs = ["vtx_python_binding"]       
        # Give away cmake code to be executed by the consumer of this package
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_python_binding_copy_files.cmake"]) 
