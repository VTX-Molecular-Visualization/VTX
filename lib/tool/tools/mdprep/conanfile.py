import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.cmake import CMakeDeps
from conan.tools.files import copy
from conan.tools.cmake import CMakeToolchain

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
        self.requires("qt/6.6.0", transitive_headers=True)
        self.requires("dylib/2.2.1")

    def generate(self):
        
        copy(self, "*.dll", self.dependencies["vtx_ui"].cpp_info.bindir, os.path.join(self.build_folder, self.cpp.build.libdirs[0]))
        copy(
            self
            , "*"
            , os.path.join(self.dependencies["vtx-gromacs"].recipe_folder, "share", "top")
            , os.path.join(self.build_folder, "data", "tool", "tools", "mdprep", "gromacs", "top")
        )
        
        
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
        copy(self, "*.dll", self.dependencies["vtx-gromacs"].build_folder, os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_mdprep"]
        # self.cpp_info.components["vtx_tool_mdprep"].requires = ["vtx-gromacs"]
        
        cmake_file = os.path.join("cmake", "qt_helper.cmake")
        self.cpp_info.set_property("cmake_build_modules", ["D:/dev/vtx/lib/tool/tools/mdprep/cmake/vtx_tool_mdprep_copy_files.cmake"])
        