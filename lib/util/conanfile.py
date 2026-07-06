from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.env import VirtualRunEnv

class VTXUtilRecipe(ConanFile):
    name = "vtx_util"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False]}
    default_options = {"shared": False, "fPIC": True, "test": False}
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "test/*"
        
    def requirements(self):
        self.requires("glm/1.0.3", transitive_headers=True)
        self.requires("spdlog/1.17.0", transitive_headers=True)
        self.requires("magic_enum/0.9.7", transitive_headers=True)
        self.requires("entt/3.16.0", transitive_headers=True)
        self.requires("assimp/6.0.5", transitive_headers=True)
        self.requires("minizip/1.3.2", force=True)
        self.requires("nlohmann_json/3.12.0")
        self.requires("cpr/1.14.2")
        self.requires("stb/cci.20240531", force=True)
        self.requires("tinyexr/1.0.7")
        self.requires("exiv2/0.28.3")
        self.requires("catch2/3.15.1")
        self.requires("platformfolders/4.3.0")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        # self.cpp.source and cpp.build are only for editable.

    def generate(self):
        VirtualRunEnv(self).generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.test == True:
            cmake.ctest(["--output-on-failure"])

    def package(self):
        cmake = CMake(self)        
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_util"]
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_configure_target.cmake", "cmake/vtx_copy.cmake"])
        # Same as self.cpp.package.includedirs in layout()
        #self.cpp_info.includedirs = []
