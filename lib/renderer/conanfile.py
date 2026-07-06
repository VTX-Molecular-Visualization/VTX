from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.env import VirtualRunEnv
from conan.tools.system.package_manager import Apt

DEFAULT_CUDA_ARCH = "native"

def install_system_dependencies(conanfile):
    if conanfile.settings.os == "Linux":
        apt = Apt(conanfile)
        apt.install(["libegl-dev", "libgl-dev", "libopengl-dev", "libwayland-dev"], update=True)

def config_options_ktx(conanfile):
    conanfile.options["ktx"].tools = False

class VTXRendererRecipe(ConanFile):
    name = "vtx_renderer"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False], "cuda_arch": ["ANY"]}
    default_options = {"shared": False, "fPIC": True, "test": False, "cuda_arch": DEFAULT_CUDA_ARCH}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "vendor/*", "shaders/*", "cmake/*", "test/*", "baker/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")
        self.requires("ktx/4.4.2")
        self.requires("fmt/12.1.0", force=True)
        self.requires("catch2/3.15.1")
        if self.settings.os == "Linux":
            self.requires("libffi/3.4.8", force=True)
            self.requires("wayland/1.24.0")
            
    def system_requirements(self):
        install_system_dependencies(self)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        config_options_ktx(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["VTX_CUDA_ARCH"] = self.options.cuda_arch
        tc.generate()
        VirtualRunEnv(self).generate()
        
    def layout(self):
        cmake_layout(self)
        self.cpp.source.includedirs = ["include/public"]
        
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
        self.cpp_info.libs = ["vtx_renderer"]
        if self.settings.os == "Windows":
            self.cpp_info.system_libs = ["opengl32"]
        elif self.settings.os == "Linux":
            self.cpp_info.system_libs = ["OpenGL", "EGL", "X11"]
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_renderer_copy_shaders.cmake", "cmake/vtx_link_cuda.cmake"])
