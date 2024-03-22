from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain

class VTXRendererTestRecipe(ConanFile):
    name = "vtx_renderer_test"
    version = "1.0"    
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "cmake/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("catch2/3.5.3")        
    
    def generate(self):
        tc = CMakeToolchain(self)
        dir_shaders = self.dependencies["vtx_renderer"].conf_info.get("user.myconf:dir_shaders")
        tc.cache_variables["DIR_SHADERS"] = dir_shaders
        tc.generate()
        
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        self.run("ctest --rerun-failed --output-on-failure")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_renderer_test"] 