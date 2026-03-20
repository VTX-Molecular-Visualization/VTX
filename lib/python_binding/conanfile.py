import os
import sys
import zipfile
from pathlib import Path

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.env import VirtualRunEnv
from conan.tools.files import copy

sys.path.append(str(Path(__file__).resolve().parent))
from python_version import (
    config_options_cpython as configure_cpython_options,
    configure_toolchain,
    configure_runtime_toolchain,
    define_python_binding_conf,
    get_python_version,
)

def _cmake_path(path: str) -> str:
    return path.replace("\\", "/")


def editable_runtime_root(p_conanFile: ConanFile) -> str:
    build_root = Path(p_conanFile.recipe_folder) / p_conanFile.folders.build
    if build_root.name == "build":
        return str(build_root / p_conanFile.settings.get_safe("build_type", default="Release"))
    return str(build_root)


def _packaged_runtime_root(p_conanFile: ConanFile) -> Path | None:
    package_root = getattr(p_conanFile, "package_folder", None)
    if not package_root:
        return None

    root = Path(package_root)
    if (root / "external" / "python").is_dir():
        return root

    return None


def runtime_root(p_conanFile: ConanFile) -> str:
    packaged_root = _packaged_runtime_root(p_conanFile)
    if packaged_root is not None:
        return _cmake_path(str(packaged_root))
    return _cmake_path(editable_runtime_root(p_conanFile))


def _archive_python_stdlib(source_dir: str, output_file: str) -> None:
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    with zipfile.ZipFile(output_file, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for root, _, files in os.walk(source_dir):
            for file_name in files:
                file_path = os.path.join(root, file_name)
                archive_path = os.path.relpath(file_path, source_dir)
                archive.write(file_path, archive_path)


def _copy_python_runtime(p_conanFile: ConanFile, dest_root: str) -> None:
    cpython_root = p_conanFile.dependencies["cpython"].package_folder
    runtime_dir = os.path.join(dest_root, "external", "python")
    version = get_python_version()

    if p_conanFile.settings.os == "Windows":
        copy(p_conanFile, "*", os.path.join(cpython_root, "bin", "DLLs"), os.path.join(runtime_dir, "DLLs"))
        copy(p_conanFile, "*", os.path.join(cpython_root, "bin", "Lib"), os.path.join(runtime_dir, "Lib"))
        copy(p_conanFile, "*.exe", os.path.join(cpython_root, "bin"), runtime_dir)
        copy(p_conanFile, "*.dll", os.path.join(cpython_root, "bin"), runtime_dir)
        copy(p_conanFile, "python*.dll", os.path.join(cpython_root, "bin"), dest_root)

        stdlib_dir = os.path.join(cpython_root, "bin", "lib")
        if os.path.isdir(stdlib_dir):
            _archive_python_stdlib(stdlib_dir, os.path.join(runtime_dir, version.zip_name))
    elif p_conanFile.settings.os == "Linux":
        copy(p_conanFile, "*", os.path.join(cpython_root, "bin"), os.path.join(runtime_dir, "bin"))
        copy(p_conanFile, "*", os.path.join(cpython_root, "lib"), os.path.join(runtime_dir, "lib"))

        stdlib_dir = os.path.join(cpython_root, "lib", f"python{version.short}")
        if os.path.isdir(stdlib_dir):
            _archive_python_stdlib(stdlib_dir, os.path.join(runtime_dir, version.zip_name))
    else:
        raise RuntimeError(f"Unsupported platform for Python runtime copy: {p_conanFile.settings.os}")


def do_python_copies(p_conanFile: ConanFile) -> None:
    _copy_python_runtime(p_conanFile, editable_runtime_root(p_conanFile))


def configureToolChain(tc: CMakeToolchain):
    configure_toolchain(tc, get_python_version())


class VTXPythonBindingRecipe(ConanFile):
    name = "vtx_python_binding"
    version = "1.0"
    package_type = "library"
    exports = "python_version.py"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False]}
    default_options = {"shared": False, "fPIC": True, "test": False}

    generators = "CMakeDeps"

    exports_sources = "CMakeLists.txt", "src/*", "module/*", "include/*", "cmake/*", "python_script/*", "test/*"

    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_io/1.0")
        self.requires("pybind11/2.13.6", transitive_headers=True)
        self.requires("catch2/3.13.0")
        self.requires("cpython/{}".format(str(get_python_version())))
        if self.settings.os == "Linux":
            self.requires("libffi/3.4.8", override=True)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

        configure_cpython_options(self)

    def generate(self):
        tc = CMakeToolchain(self)
        configureToolChain(tc)
        configure_runtime_toolchain(tc, runtime_root(self))
        tc.generate()
        VirtualRunEnv(self).generate()
        do_python_copies(self)

        dep_name = "cpython"
        if str(self.settings.os) == "Linux" and dep_name in self.dependencies:
            dep = self.dependencies[dep_name]
            dep.cpp_info.system_libs = [l for l in dep.cpp_info.system_libs if l != "nsl"]
            for comp in dep.cpp_info.components.values():
                comp.system_libs = [l for l in comp.system_libs if l != "nsl"]

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        if self.options.test == True:
            self.run("ctest --output-on-failure", cwd=self.build_folder, env="conanrun")

    def package(self):
        cmake = CMake(self)
        cmake.install()
        _copy_python_runtime(self, self.package_folder)

    def package_info(self):
        self.cpp_info.libs = ["vtx_python_binding"]
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_python_binding_copy_runtime.cmake"])
        define_python_binding_conf(
            self.conf_info,
            get_python_version(),
            runtime_root=runtime_root(self),
        )
