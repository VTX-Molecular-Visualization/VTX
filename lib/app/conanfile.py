import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain


def import_module_from_file(file_path):
    file_path = str(Path(__file__).resolve().parent / file_path)
    spec = importlib.util.spec_from_file_location("conanfile.py", file_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module
    

python_binding_module = import_module_from_file( Path("..") / "python_binding" / "conanfile.py" )

class VTXAppRecipe(ConanFile):
    name = "vtx_app"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False], "version": ["ANY"]}
    default_options = {"shared": False, "fPIC": True, "test": False, "version": "0.0.0" }
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "vendor/*", "test/*", "data/*"

    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("catch2/3.11.0")
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        self.cpp.source.libdirs.append(os.path.join("vendor", "velopack", "lib-static"))
         
    def generate(self):
        tc = CMakeToolchain(self)
        python_binding_module.config_options_cpython(tc)
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
        self.cpp_info.libdirs.append(os.path.join("vendor", "velopack", "lib-static"))
        if self.settings.os == "Windows":
            self.cpp_info.libs.append("velopack_libc_win_x64_msvc")
            self.cpp_info.system_libs.append("ntdll")
        else:
            self.cpp_info.libs.append("velopack_libc_linux_x64_gnu")
           
