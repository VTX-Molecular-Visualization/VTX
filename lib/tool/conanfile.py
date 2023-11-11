from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class VTXToolRecipe(ConanFile):
    name = "vtx_tool"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "asset/*"
        
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_ui/1.0")
        self.requires("eigen/3.4.0")
        self.requires("qt/6.6.0", transitive_headers=True)            
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool"]

