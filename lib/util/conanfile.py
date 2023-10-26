from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class VTXUtilRecipe(ConanFile):
    name = "vtx_util"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    #TODO: needed?
    #exports_sources = "CMakeLists.txt", "src/*", "include/*"
        
    def requirements(self):
        self.requires("glm/0.9.9.8", transitive_headers=True)
        #TODO: hide impl to remove transitive_headers.
        self.requires("spdlog/1.12.0", transitive_headers=True)
        self.requires("nlohmann_json/3.11.2")
        self.requires("magic_enum/0.9.3", transitive_headers=True)
        self.requires("asio/1.28.1")
     
    #TODO: get CMake?
    #def build_requirements(self):
        
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
        self.cpp_info.libs = ["vtx_util"]

