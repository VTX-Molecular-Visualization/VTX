from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain, CMakeDeps

class VTXPythonBindingTestRecipe(ConanFile):
    name = "vtx_python_binding_test"
    version = "1.0"    
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "data/*", "cmake/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("catch2/3.7.1")

    def generate(self):
        deps = CMakeDeps(self)
        deps.check_components_exist = True
        deps.generate()    

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.ctest(["--output-on-failure"])

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_python_binding_test"] 