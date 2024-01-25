import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.files import copy

class VTXUiRecipe(ConanFile):
    name = "vtx_ui"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "asset/*"
        
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("qt/6.6.1", transitive_headers=True)
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)      

    def generate(self):
        tc = CMakeToolchain(self)
        dir_python_script = self.dependencies["vtx_python_binding"].conf_info.get("user.myconf:dir_python_script")
        tc.cache_variables["DIR_PYTHON_SCRIPT"] = dir_python_script
        path_python_module = self.dependencies["vtx_python_binding"].conf_info.get("user.myconf:path_python_module")
        tc.cache_variables["PATH_PYTHON_MODULE"] = path_python_module
        tc.generate()
        
        copy(self, "*.cmake", self.source_folder, self.build_folder)
        copy(self, "*.dll", self.dependencies["qt"].cpp_info.bindir, os.path.join(self.build_folder, self.cpp.build.libdirs[0]))
        copy(self, "*.dll", os.path.join(self.dependencies["qt"].package_folder, "res/archdatadir/plugins"), os.path.join(self.build_folder, self.cpp.build.libdirs[0]))

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.cmake", self.build_folder, self.package_folder)
        copy(self, "*.dll", self.build_folder, os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = ["vtx_ui"]
        cmake_file = os.path.join("cmake", "qt_helper.cmake")
        self.cpp_info.set_property("cmake_build_modules", [cmake_file])
        if self.settings.os == "Windows":
            self.cpp_info.system_libs.append('d3d12')