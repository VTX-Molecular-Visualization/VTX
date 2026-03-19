import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy

class VTXRecipe(ConanFile):
    name = "vtx"
    version = "1.0"
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"version": ["ANY"], "tool_example": [True, False], "tool_mdprep": [True, False]}
    default_options = {"version": "0.0.0", "tool_example": False, "tool_mdprep": True }

    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "include/*", "src/*", "asset/*", "data/*", "cmake/*", "internal_data/*", "libraries/*", "CHANGELOG.md", "README.md", "license.txt"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_ui_qt/1.0")
        if self.options.tool_example:
            self.requires("vtx_tool_example/1.0")
        if self.options.tool_mdprep:
            self.requires("vtx_tool_mdprep/1.0")

    def layout(self):
        cmake_layout(self)
        
    def generate(self):
        tc = CMakeToolchain(self)
        versionMajor, versionMinor, versionPatch = map(int, str(self.dependencies["vtx_app"].options.version).split('.'))
        tc.cache_variables["VTX_VERSION_MAJOR"] = versionMajor
        tc.cache_variables["VTX_VERSION_MINOR"] = versionMinor
        tc.cache_variables["VTX_VERSION_PATCH"] = versionPatch 
        tc.cache_variables["VTX_TOOL_EXAMPLE"] = 1 if self.options.tool_example else 0
        tc.cache_variables["VTX_TOOL_MDPREP"] = 1 if self.options.tool_mdprep else 0
        tc.generate()

        copy(self, "*.dll", self.dependencies["vtx_ui_qt"].cpp_info.bindirs[0], os.path.join(self.build_folder, self.cpp.build.libdirs[0]))

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx"]
