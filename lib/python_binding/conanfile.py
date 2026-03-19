import os
import zipfile
import glob
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conan.tools.env import VirtualBuildEnv, VirtualRunEnv, Environment
from conan.tools.files import copy
from conan.tools.microsoft import is_msvc
from pathlib import Path
import sys

sys.path.append(str(Path(__file__).resolve().parent))
from python_version import (
    config_options_cpython as configure_cpython_options,
    configure_toolchain,
    define_python_binding_conf,
    get_python_version,
)

def executable_folder(p_conanFile: ConanFile):
    if Path(p_conanFile.build_folder).name == "build":
        return str(Path(p_conanFile.build_folder) / p_conanFile.settings.get_safe("build_type", default="Release"))
    else:
        return p_conanFile.build_folder


def create_python_stdlib_zip(p_conanFile: ConanFile, python_lib_path, zip_destination):
    """Create the Python standard library zip from the packaged interpreter."""
    p_conanFile.output.info(f"Creating {Path(zip_destination).name} from {python_lib_path} to {zip_destination}")

    with zipfile.ZipFile(zip_destination, 'w', zipfile.ZIP_DEFLATED) as zipf:
        lib_path = Path(python_lib_path)
        if lib_path.exists():
            for file_path in lib_path.rglob('*.py'):
                # Calculate the archive name (relative path from lib directory)
                arcname = file_path.relative_to(lib_path)
                zipf.write(file_path, arcname)

            # Also include .pyc files if they exist
            for file_path in lib_path.rglob('*.pyc'):
                arcname = file_path.relative_to(lib_path)
                zipf.write(file_path, arcname)

            p_conanFile.output.info(f"Successfully created {Path(zip_destination).name} with {len(zipf.namelist())} files")
        else:
            p_conanFile.output.warning(
                f"Python lib path {python_lib_path} does not exist, skipping {Path(zip_destination).name} creation"
            )


def _doPythonCopies(p_conanFile: ConanFile, dest):
    p_conanFile.output.info("Doing python-related copy.")
    if p_conanFile.settings.os == "Windows":
        p_conanFile.output.info("Copying from <{}> to <{}>".format(
            os.path.join(p_conanFile.dependencies["cpython"].package_folder,"bin")
            ,  os.path.join(dest, "external","python")
        ))
        
        DLLs_path = Path(p_conanFile.dependencies["cpython"].package_folder,"bin", "DLLs")
        if not DLLs_path.exists():
            p_conanFile.output.info("Directory <{}> necessary but not found".format(str(DLLs_path)))
        for subdir in ("DLLs","Lib"):
            copy(p_conanFile, "*", os.path.join(p_conanFile.dependencies["cpython"].package_folder,"bin", subdir), os.path.join(dest, "external","python",subdir))  
        copy(p_conanFile, "*.exe", os.path.join(p_conanFile.dependencies["cpython"].package_folder,"bin"), os.path.join(dest, "external","python"))  
        copy(p_conanFile, "*.dll", os.path.join(p_conanFile.dependencies["cpython"].package_folder,"bin"), os.path.join(dest, "external","python"))  
        copy(p_conanFile, "*.dll", os.path.join(p_conanFile.dependencies["cpython"].package_folder,"bin"), os.path.join(dest))        
    else:
        for subdir in ("bin","lib"):
            copy(p_conanFile, "*", os.path.join(p_conanFile.dependencies["cpython"].package_folder, subdir), os.path.join(dest, "external","python",subdir))  
    
    python_version = get_python_version()

    # Create the Python standard library zip from the configured interpreter version
    if p_conanFile.settings.os == "Windows":
        python_lib_path = os.path.join(p_conanFile.dependencies["cpython"].package_folder,"bin", "lib")
    else:
        python_lib_path = os.path.join(p_conanFile.dependencies["cpython"].package_folder, "lib")
    zip_destination = os.path.join(dest, "external", "python", python_version.zip_name)
    create_python_stdlib_zip(p_conanFile, python_lib_path, zip_destination)

def doPythonCopies(p_conanFile: ConanFile):
    _doPythonCopies(p_conanFile, executable_folder(p_conanFile))


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
        self.requires("cpython/{}".format(str(get_python_version()))) # v >= 3.10 not working with msvc compiler so far
        if self.settings.os == "Linux":
            self.requires("libffi/3.4.8", override=True)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        
        configure_cpython_options(self)
                
    def generate(self):
        tc = CMakeToolchain(self)
        configureToolChain(tc)
        tc.generate()
        VirtualRunEnv(self).generate()
        
        dep_name = "cpython"
        if str(self.settings.os) == "Linux" and dep_name in self.dependencies:
            dep = self.dependencies[dep_name]
            # Root cpp_info
            dep.cpp_info.system_libs = [l for l in dep.cpp_info.system_libs if l != "nsl"]
            # Components 
            for comp in dep.cpp_info.components.values():
                comp.system_libs = [l for l in comp.system_libs if l != "nsl"]
                
        doPythonCopies(self) 
        
    def _print_dir_content(self, p_dir):
        # Print build folder contents for debugging
        self.output.info("=== Build folder contents before running tests ===")
        try:
            for root, dirs, files in os.walk(p_dir):
                level = root.replace(self.build_folder, '').count(os.sep)
                indent = ' ' * 2 * level
                self.output.info(f"{indent}{os.path.basename(root)}/")
                subindent = ' ' * 2 * (level + 1)
                for file in files:
                    self.output.info(f"{subindent}{file}")
        except Exception as e:
            self.output.warning(f"Failed to list build folder contents: {e}")
        self.output.info("=== End build folder contents ===")
    
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()

        cmake.build()

        if self.options.test == True:
            # self._print_dir_content(executable_folder(self))
            self.run("ctest --output-on-failure", cwd=self.build_folder, env="conanrun")

    def package(self):
        cmake = CMake(self)
        cmake.install()
        _doPythonCopies(self, self.package_folder) 

    def package_info(self):
        self.cpp_info.libs = ["vtx_python_binding"]
        # Give away cmake code to be executed by the consumer of this package
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_python_binding_copy_runtime.cmake"])
        define_python_binding_conf(self.conf_info, get_python_version())
