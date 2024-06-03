import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy

class VTXUiTestRecipe(ConanFile):
    name = "vtx_ui_test"
    version = "1.0"    
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
        
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "data/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("vtx_ui/1.0")
        self.requires("catch2/3.6.0")
        
    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        dir_shaders = self.dependencies["vtx_renderer"].conf_info.get("user.myconf:dir_shaders")
        tc.cache_variables["DIR_SHADERS"] = dir_shaders
        dir_python_script = self.dependencies["vtx_python_binding"].conf_info.get("user.myconf:dir_python_script")
        tc.cache_variables["DIR_PYTHON_SCRIPT"] = dir_python_script
        path_python_module = self.dependencies["vtx_python_binding"].conf_info.get("user.myconf:path_python_module")
        tc.cache_variables["PATH_PYTHON_MODULE"] = path_python_module
        tc.generate()

        copy(self, "*.dll", self.dependencies["vtx_ui"].cpp_info.bindir, os.path.join(self.build_folder, self.cpp.build.libdirs[0]))
            
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.dll", self.build_folder, os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = ["vtx_ui_test"]