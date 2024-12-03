from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain

class VTXAppRecipe(ConanFile):
    name = "vtx_app"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False]}
    default_options = {"shared": False, "fPIC": True, "test": False}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "test/*", "data/*"

    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_core/1.0")
        self.requires("entt/3.13.2", transitive_headers=True)
        self.requires("catch2/3.7.0")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        
        self.cpp.build.components["vtx_app"].libdirs = self.cpp.build.libdirs
        self.cpp.build.components["vtx_app_no_opengl"].libdirs = self.cpp.build.libdirs
        
    def generate(self):
        tc = CMakeToolchain(self)
        dir_shaders = self.dependencies["vtx_renderer"].conf_info.get("user.myconf:dir_shaders")
        tc.cache_variables["DIR_SHADERS"] = dir_shaders
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
        self.cpp_info.components["vtx_app_no_opengl"].libs = ["vtx_app_no_opengl"]
        self.cpp_info.components["vtx_app_no_opengl"].set_property("cmake_target_name", "vtx_app::vtx_app_no_opengl")
        self.cpp_info.components["vtx_app"].libs = ["vtx_app"]
        self.cpp_info.components["vtx_app"].set_property("cmake_target_name", "vtx_app::vtx_app")

