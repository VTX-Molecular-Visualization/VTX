import os
from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.env import VirtualRunEnv
from conan.tools.system.package_manager import Apt


def velopack_lib_name(os_name: str, arch: str) -> str:
    if os_name == "Windows":
        if arch == "x86_64":
            return "velopack_libc_win_x64_msvc.lib"
        if arch == "armv8":
            return "velopack_libc_win_arm64_msvc.lib"
    elif os_name == "Linux":
        if arch == "x86_64":
            return "libvelopack_libc_linux_x64_gnu.a"
        if arch == "armv8":
            return "libvelopack_libc_linux_arm64_gnu.a"
    elif os_name == "Macos":
        if arch == "x86_64":
            return "velopack_libc_osx_x64_gnu.a"
        if arch == "armv8":
            return "velopack_libc_osx_arm64_gnu.a"

    raise ConanInvalidConfiguration(f"Unsupported Velopack platform: {os_name}/{arch}")


def _conan_lib_name(filename: str) -> str:
    name, _ = os.path.splitext(filename)
    if name.startswith("lib"):
        return name[3:]
    return name


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
            self.requires("vtx_renderer/1.0", transitive_headers=True) # Need for transitive header as action/visibility ends up including renderer stuff
        if self.options.python_binding:
            self.requires("vtx_python_binding/1.0")
        self.requires("catch2/3.15.1")
        self.requires("argparse/3.2")
        

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
        tc.cache_variables["VTX_VELOPACK_LIB"] = velopack_lib_name(
            str(self.settings.os), str(self.settings.arch)
        )
        tc.generate()
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
        os_name = str(self.settings.os)
        arch = str(self.settings.arch)
        velopack_lib_filename = velopack_lib_name(os_name, arch)

        self.cpp_info.libs = ["vtx_app"]        
        self.cpp_info.libdirs.append(os.path.join("vendor", "velopack", "lib-static"))
        self.conf_info.define("user.app:renderer", "1" if self.options.renderer else "0")
        self.conf_info.define("user.app:python_binding", "1" if self.options.python_binding else "0")
        self.cpp_info.libs.append(_conan_lib_name(velopack_lib_filename))
        if os_name == "Windows":
            self.cpp_info.system_libs.append("ntdll")
