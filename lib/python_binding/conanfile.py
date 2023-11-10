import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class VTXPythonBindingRecipe(ConanFile):
    name = "vtx_python_binding"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "python_script/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_io/1.0")
        self.requires("pybind11/2.11.1")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)        
        self.cpp.build.components["vtx_python_binding"].libdirs = ['.']
        self.cpp.build.components["pytx"].libdirs = ['.']

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components["vtx_python_binding"].libs = ["vtx_python_binding"]
        self.cpp_info.components["pytx"].libs = ["pytx"]
        self.cpp_info.components["pytx"].set_property("cmake_target_name", "PyTX")
        self.conf_info.define("user.myconf:dir_python_script", os.path.join(self.package_folder + "/python_script"))
        self.conf_info.define("user.myconf:path_python_module", os.path.join(self.package_folder + "/bin/PyTX.cp311-win_amd64.pyd"))