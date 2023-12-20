import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy

class VTXUtilRecipe(ConanFile):
    name = "vtx_util"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*"
        
    def requirements(self):
        self.requires("glm/0.9.9.8", transitive_headers=True)
        self.requires("spdlog/1.12.0", transitive_headers=True)
        self.requires("nlohmann_json/3.11.2")
        self.requires("magic_enum/0.9.3", transitive_headers=True)
        self.requires("asio/1.29.0")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        # self.cpp.source and cpp.build are only for editable.

    def generate(self):
        copy(self, "*.cmake", self.source_folder, self.build_folder)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "*.cmake", self.build_folder, self.package_folder)

    def package_info(self):
        self.cpp_info.libs = ["vtx_util"]
        cmake_file = os.path.join("cmake", "configure_target.cmake")
        self.cpp_info.set_property("cmake_build_modules", [cmake_file])
        # Same as self.cpp.package.includedirs in layout()
        #self.cpp_info.includedirs = []