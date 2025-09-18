import os
import zipfile
import glob
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conan.tools.env import VirtualBuildEnv, Environment
from conan.tools.files import copy
from pathlib import Path

class VTXPythonBindingRecipe(ConanFile):    
    name = "vtx_python_binding"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False]}
    default_options = {"shared": False, "fPIC": True, "test": False}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "module/*", "include/*", "cmake/*", "python_script/*", "test/*"
    
    _former_LD_LIBRARY_PATH = None
    
    def _generated_cmake_prefix(self):
        return "pybind11-"
        
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")  
        self.requires("vtx_io/1.0")
        self.requires("pybind11/2.13.6", transitive_headers=True)
        self.requires("catch2/3.8.1")
        self.requires("cpython/3.9.19") # v >= 3.10 not working with msvc compiler so far
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

        self.options["cpython"].with_gdbm = False # Doesn't work on windows. I'm not sure what it does.
        self.options["cpython"].shared = self.settings.os == "Windows" # False by default. If set to False, DLLs will be missing.
        
    def _create_python39_zip(self, python_lib_path, zip_destination):
        """Create python39.zip from the Python standard library"""
        self.output.info(f"Creating python39.zip from {python_lib_path} to {zip_destination}")

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

                self.output.info(f"Successfully created python39.zip with {len(zipf.namelist())} files")
            else:
                self.output.warning(f"Python lib path {python_lib_path} does not exist, skipping python39.zip creation")
                
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()      
        
        
        if self.settings.os == "Windows":
            for subdir in ("DLLs","Lib"):
                copy(self, "*", os.path.join(self.dependencies["cpython"].package_folder,"bin", subdir), os.path.join(self.build_folder, "external","python",subdir))  
            copy(self, "*.dll", os.path.join(self.dependencies["cpython"].package_folder,"bin"), os.path.join(self.build_folder))        
            # Create python39.zip from the Python standard library
            python_lib_path = os.path.join(self.dependencies["cpython"].package_folder, "lib")
            zip_destination = os.path.join(self.build_folder, "python39.zip")
            self._create_python39_zip(python_lib_path, zip_destination)
        else:
            for subdir in ("bin","lib"):
                copy(self, "*", os.path.join(self.dependencies["cpython"].package_folder, subdir), os.path.join(self.build_folder, "external","python",subdir))  
            # Create python39.zip from the Python standard library (Linux/macOS)
            python_lib_path = os.path.join(self.dependencies["cpython"].package_folder, "lib", "python3.9")
            zip_destination = os.path.join(self.build_folder, "python39.zip")
            self._create_python39_zip(python_lib_path, zip_destination)   
        
    
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()

        cmake.build()

        if self.options.test == True:
            # Print build folder contents for debugging
            self.output.info("=== Build folder contents before running tests ===")
            try:
                for root, dirs, files in os.walk(self.build_folder):
                    level = root.replace(self.build_folder, '').count(os.sep)
                    indent = ' ' * 2 * level
                    self.output.info(f"{indent}{os.path.basename(root)}/")
                    subindent = ' ' * 2 * (level + 1)
                    for file in files:
                        self.output.info(f"{subindent}{file}")
            except Exception as e:
                self.output.warning(f"Failed to list build folder contents: {e}")
            self.output.info("=== End build folder contents ===")

            cmake.ctest(["--output-on-failure"])

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_python_binding"]       
        # Give away cmake code to be executed by the consumer of this package
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_python_binding_copy_files.cmake"]) 
