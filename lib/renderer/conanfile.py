import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy

class VTXRendererRecipe(ConanFile):
    name = "vtx_renderer"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "shaders/*", "cmake/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("glad/0.1.36", transitive_headers=True)
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        
        self.options["glad/*"].shared = False
        self.options["glad/*"].spec = "gl"
        self.options["glad/*"].gl_profile = "core"
        self.options["glad/*"].gl_version = 4.5
        self.options["glad/*"].extensions = ["GL_NVX_gpu_memory_info"]

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.cmake", self.build_folder, self.package_folder)

    def package_info(self):
        self.cpp_info.libs = ["vtx_renderer"]
        self.conf_info.define("user.myconf:dir_shaders", os.path.join(self.package_folder, "shaders"))
        self.cpp_info.set_property("cmake_build_modules", ["cmake/copy_shaders.cmake"])
        