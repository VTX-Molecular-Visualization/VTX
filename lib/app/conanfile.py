import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.env import VirtualRunEnv
from conan.tools.system.package_manager import Apt


class VTXAppRecipe(ConanFile):
    name = "vtx_app"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "test": [True, False],
        "version": ["ANY"],
        "renderer": [True, False],
        "python_binding": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "test": False,
        "version": "0.0.0",
        "renderer": True,
        "python_binding": True,
    }
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "vendor/*", "test/*", "data/*"

    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_io/1.0")
        self.requires("vtx_core/1.0")
        if self.options.renderer:
            self.requires("vtx_renderer/1.0")
        if self.options.python_binding:
            self.requires("vtx_python_binding/1.0")
        self.requires("catch2/3.13.0")
        if self.settings.os == "Linux":
            self.requires("libffi/3.4.8", override=True)

    def system_requirements(self):
        if self.settings.os == "Linux":
            apt = Apt(self)
            apt.install(["libnsl-dev"], update=True)
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        self.cpp.source.libdirs.append(os.path.join("vendor", "velopack", "lib-static"))
         
    def generate(self):
        tc = CMakeToolchain(self)
        if self.options.python_binding:
            python_binding_conf = self.dependencies["vtx_python_binding"].conf_info
            tc.cache_variables["CPYTHON_VERSION_MAJOR"] = python_binding_conf.get("user.python_binding:cpython_version_major")
            tc.cache_variables["CPYTHON_VERSION_MINOR"] = python_binding_conf.get("user.python_binding:cpython_version_minor")
            tc.cache_variables["CPYTHON_VERSION_PATCH"] = python_binding_conf.get("user.python_binding:cpython_version_patch")
            tc.cache_variables["VTX_PYTHON_BINDING_RUNTIME_ROOT"] = python_binding_conf.get("user.python_binding:runtime_root")
        versionMajor, versionMinor, versionPatch = map(int, str(self.options.version).split('.'))
        tc.cache_variables["VTX_VERSION_MAJOR"] = versionMajor
        tc.cache_variables["VTX_VERSION_MINOR"] = versionMinor
        tc.cache_variables["VTX_VERSION_PATCH"] = versionPatch
        tc.cache_variables["VTX_RENDERER"] = 1 if self.options.renderer else 0
        tc.cache_variables["VTX_PYTHON_BINDING"] = 1 if self.options.python_binding else 0
        tc.generate()
        VirtualRunEnv(self).generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.test == True:
            self.run("ctest --output-on-failure", cwd=self.build_folder, env="conanrun")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_app"]        
        self.cpp_info.libdirs.append(os.path.join("vendor", "velopack", "lib-static"))
        self.conf_info.define("user.app:renderer", "1" if self.options.renderer else "0")
        self.conf_info.define("user.app:python_binding", "1" if self.options.python_binding else "0")
        if self.settings.os == "Windows":
            self.cpp_info.libs.append("velopack_libc_win_x64_msvc")
            self.cpp_info.system_libs.append("ntdll")
        else:
            self.cpp_info.libs.append("velopack_libc_linux_x64_gnu")
           
