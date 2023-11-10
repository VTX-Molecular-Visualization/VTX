from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain

class VTXPythonBindingTestRecipe(ConanFile):
    name = "vtx_python_binding_test"
    version = "1.0"    
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "cmake/*", "data/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("catch2/3.4.0")   

    def generate(self):
        tc = CMakeToolchain(self)
        dir_python_script = self.dependencies["vtx_python_binding"].conf_info.get("user.myconf:dir_python_script")
        tc.cache_variables["DIR_PYTHON_SCRIPT"] = dir_python_script
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
        self.cpp_info.libs = ["vtx_python_binding_test"] 