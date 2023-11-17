import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy

import shutil
from pathlib import Path

def move_dlls__ui(dest: Path) :
    """
    Assumes that dependencies dlls have been copied into default location beforehand
        Default location should be the same directory tree that the lib itself, relative to this file's folder
    """
    for file in (dest / "lib" / "ui" / "build").iterdir():
        if (file.is_dir()) :
            shutil.copytree(file, dest / file.name)
        elif (file.is_file()) :
            shutil.copy2(file,  dest / file.name)
    shutil.rmtree(dest / "lib" / "ui") # cleanup
    

class VTXRecipe(ConanFile):
    name = "vtx"
    version = "1.0"
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "include/*", "src/*", "cmake/*", "asset/*", "internal_data/*", "libraries/*", "CHANGELOG.md", "README.md", "license.txt"
    
    def requirements(self):
        self.requires("vtx_app/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_tool/1.0")
        self.requires("vtx_ui/1.0")
        self.requires("vtx_util/1.0")

    def generate(self):
        tc = CMakeToolchain(self)
        dir_shaders = self.dependencies["vtx_renderer"].conf_info.get("user.myconf:dir_shaders")
        tc.cache_variables["DIR_SHADERS"] = dir_shaders
        dir_python_script = self.dependencies["vtx_python_binding"].conf_info.get("user.myconf:dir_python_script")
        tc.cache_variables["DIR_PYTHON_SCRIPT"] = dir_python_script
        path_python_module = self.dependencies["vtx_python_binding"].conf_info.get("user.myconf:path_python_module")
        tc.cache_variables["PATH_PYTHON_MODULE"] = path_python_module
        tc.generate()

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        
        # Copy dll from dependencies into exe dir into [exe_dir]/lib/[module]/build/[build_type]
        build_type = self.settings.get_safe("build_type", default="Release")
        exe_dir = os.path.join(self.build_folder, build_type)
        copy(self, "*.dll", self.package_folder, exe_dir)

        # Put dependencies dlls into exe dir
        move_dlls__ui(Path(exe_dir))

        shutil.rmtree(Path(exe_dir) / "lib" ) # cleanup

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.dll", self.build_folder, os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = ["vtx"]
