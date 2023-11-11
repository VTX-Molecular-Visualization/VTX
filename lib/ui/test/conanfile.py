from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.files import copy

class VTXUiTestRecipe(ConanFile):
    name = "vtx_ui_test"
    version = "1.0"    
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
        
    generators = "CMakeToolchain", "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "cmake/*", "data/*"
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_ui/1.0")
        self.requires("catch2/3.4.0")
        
    def layout(self):
        cmake_layout(self)

    #def generate(self):
        #for dep in self.dependencies.values():
        #    copy(self, "*.dylib", dep.cpp_info.libdir, self.build_folder)
        #    copy(self, "*.dll", dep.cpp_info.libdir, self.build_folder)
            
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        #self.run("ctest --rerun-failed --output-on-failure")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_ui_test"] 