import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.cmake import CMakeDeps
from conan.tools.files import copy
from conan.tools.cmake import CMakeToolchain
from pathlib import Path

class VTXToolMdprepRecipe(ConanFile):
    name = "vtx_tool_mdprep"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def _generated_cmake_prefix(self):
        return "gmxbin-"
     
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_ui/1.0")      
        self.requires("vtx-gromacs/2024.0")      
        self.requires("qt/6.6.0", transitive_headers=True)
        self.requires("dylib/2.2.1")

    def generate(self):
        copy(self, "*.dll", self.dependencies["vtx_ui"].cpp_info.bindir, os.path.join(self.build_folder, self.cpp.build.libdirs[0]))
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        self.cpp.build.libdirs = self.cpp.build.libdirs
        self.cpp.source.includedirs = self.cpp.source.includedirs

    def build(self):
        gmx_bin_dir = os.path.join(self.dependencies["vtx-gromacs"].cpp_info.bindir, "bin")
        gmx_bin_dest = os.path.join(self.build_folder, self.cpp.build.libdirs[0], "external", "tools", "mdprep", "gromacs")
        for ext in ("*.dll", "*.a", "*.so", "*.exe", "*.dylib", "^[^.]$"): # No extension for executable in linux, right ?
            copy(self, ext, gmx_bin_dir, gmx_bin_dest)
        
        '''
        cmake_dir = os.path.join(self.recipe_folder, "cmake")
        cmake_file_name = f"{self._generated_cmake_prefix()}{self.settings.build_type}.cmake"
        cmake_file_path = os.path.join(cmake_dir, cmake_file_name)
        cmake_file_content = """register_build_directory_copy("%s" "external/tools/mdprep/gromacs")""" % (Path(gmx_bin_dest).as_posix())
        
        Path(cmake_file_path).write_text(cmake_file_content)
        print("Creating .cmake at <%s>" % cmake_file_path)
        print("self.settings.build_type : <%s>" % self.settings.build_type)
        '''
        # gmx_bin_dest = os.path.join(self.build_folder, self.cpp.build.libdirs[0], "external", "tools", "mdprep", "gromacs")
        # print("copy <%s> into <%s>" % (gmx_bin_dir, gmx_bin_dest)) 
        
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.dll", (self.dependencies["vtx-gromacs"].build_folder), os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_mdprep"]
        # Give away cmake code to be executed by the consumer of this package
        # generated_cmake = "cmake/%s%s.cmake" % (self._generated_cmake_prefix(), self.settings.build_type)
        # self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_tool_mdprep_copy_files.cmake", generated_cmake])
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_tool_mdprep_copy_files.cmake"])
        