import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.cmake import CMakeToolchain
from conan.tools.env import VirtualRunEnv

class VTXToolMdprepRecipe(ConanFile):
    name = "vtx_tool_mdprep"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False]}
    default_options = {"shared": False, "fPIC": True, "test": False}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "test/*", "data/*", "asset/*"

     
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_ui_qt/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("re2/20240702")
        self.requires("gromacs/2026.0")
        self.requires("catch2/3.13.0")
        if self.settings.os == "Linux":
            self.requires("libffi/3.4.8", override=True)
            self.requires("wayland/1.24.0", override=True)            

    def generate(self):
        tc = CMakeToolchain(self)
        python_binding_conf = self.dependencies["vtx_python_binding"].conf_info
        tc.cache_variables["CPYTHON_VERSION_MAJOR"] = python_binding_conf.get("user.python_binding:cpython_version_major")
        tc.cache_variables["CPYTHON_VERSION_MINOR"] = python_binding_conf.get("user.python_binding:cpython_version_minor")
        tc.cache_variables["CPYTHON_VERSION_PATCH"] = python_binding_conf.get("user.python_binding:cpython_version_patch")
        tc.generate()
        VirtualRunEnv(self).generate()
        
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
        if self.options.test == True:
            self.run("ctest --output-on-failure", cwd=self.build_folder, env="conanrun")
        
    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_mdprep"]
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_tool_mdprep_copy_data.cmake"])
        
