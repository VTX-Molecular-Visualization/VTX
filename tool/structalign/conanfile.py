import os
import shutil
from pathlib import Path
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.cmake import CMakeToolchain
from conan.tools.files import copy

_CONF_RUNTIME_ROOT = "user.tool_structalign:runtime_root"


def _cmake_path(path: str) -> str:
    return path.replace("\\", "/")


class VTXToolStructAlignRecipe(ConanFile):
    name = "vtx_tool_structalign"
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
        self.requires("vtx_core/1.0")
        self.requires("vtx_ui_qt/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("catch2/3.15.1")

    def generate(self):
        tc = CMakeToolchain(self)
        app_conf = self.dependencies["vtx_app"].conf_info
        python_binding_conf = self.dependencies["vtx_python_binding"].conf_info
        tc.cache_variables["CPYTHON_VERSION_MAJOR"] = python_binding_conf.get("user.python_binding:cpython_version_major")
        tc.cache_variables["CPYTHON_VERSION_MINOR"] = python_binding_conf.get("user.python_binding:cpython_version_minor")
        tc.cache_variables["CPYTHON_VERSION_PATCH"] = python_binding_conf.get("user.python_binding:cpython_version_patch")
        tc.cache_variables["VTX_PYTHON_BINDING_RUNTIME_ROOT"] = python_binding_conf.get("user.python_binding:runtime_root")
        qt_conf = self.dependencies["vtx_ui_qt"].conf_info
        tc.cache_variables["VTX_QT_RUNTIME_ROOT"] = qt_conf.get("user.ui_qt:runtime_root")
        tc.cache_variables["VTX_UI_QT"] = "1"
        tc.cache_variables["VTX_RENDERER"] = app_conf.get("user.app:renderer")
        tc.cache_variables["VTX_PYTHON_BINDING"] = app_conf.get("user.app:python_binding")
        tc.generate()
        do_gromacs_copies(self)
        
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
            cmake.ctest([ "--output-on-failure"])

    def package(self):
        cmake = CMake(self)
        cmake.install()
        _copy_gromacs_runtime(self, self.package_folder)

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_structalign"]
        self.conf_info.define(_CONF_RUNTIME_ROOT, runtime_root(self))
        
