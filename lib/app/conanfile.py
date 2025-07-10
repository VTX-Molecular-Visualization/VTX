from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain

class VTXAppRecipe(ConanFile):
    name = "vtx_app"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False], "version": ["ANY"]}
    default_options = {"shared": False, "fPIC": True, "test": False, "version": "0.0.0" }
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "test/*", "data/*"

    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("entt/3.14.0", transitive_headers=True)
        self.requires("catch2/3.8.1")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
         
    def generate(self):
        tc = CMakeToolchain(self)
        versionMajor, versionMinor, versionPatch = map(int, str(self.options.version).split('.'))
        tc.cache_variables["VTX_VERSION_MAJOR"] = versionMajor
        tc.cache_variables["VTX_VERSION_MINOR"] = versionMinor
        tc.cache_variables["VTX_VERSION_PATCH"] = versionPatch        
        tc.generate()

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
        self.cpp_info.libs = ["vtx_app"]

