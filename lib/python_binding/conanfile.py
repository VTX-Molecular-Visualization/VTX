import os
import glob
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy
from pathlib import Path

class VTXPythonBindingRecipe(ConanFile):    
    name = "vtx_python_binding"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps", "CMakeToolchain"
    # generators =  "CMakeToolchain"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/library.cmake", "cmake/vtx_python_binding_copy_files.cmake", "python_script/*", "test/*"
    
    def _generated_cmake_prefix(self):
        return "pybind11-"
        
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_app/1.0" )
        self.requires("vtx_io/1.0")
        self.requires("pybind11/2.12.0", transitive_headers=True)
        self.requires("catch2/3.7.0")
        
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        
    def generate(self):
        copy(self, "*.cmake", self.source_folder, self.build_folder)
        
    # def generate(self):
        # deps = CMakeDeps(self)
        # deps.check_components_exist = True
        # deps.generate()
    
    def layout(self):
        cmake_layout(self)

        self.cpp.build.components["vtx_python_binding"].libdirs = self.cpp.build.libdirs
        self.cpp.build.components["vtx_python_bin"].libdirs = self.cpp.build.libdirs
        self.cpp.build.components["vtx_python_binding_test"].libdirs = self.cpp.build.libdirs

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.ctest(["--output-on-failure"])

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components["vtx_python_binding"].libs = ["vtx_python_binding"]
        self.cpp_info.components["vtx_python_binding"].set_property("cmake_target_name", "vtx_python_binding::vtx_python_binding")
        self.cpp_info.components["vtx_python_binding"].requires =["vtx_util::vtx_util", "vtx_core::vtx_core", "vtx_app::vtx_app", "vtx_io::vtx_io", "pybind11::pybind11", "pybind11::embed"]
        
        self.cpp_info.components["vtx_python_bin"].libs = ["vtx_python_bin"]
        self.cpp_info.components["vtx_python_bin"].set_property("cmake_target_name", "vtx_python_binding::vtx_python_bin")
        self.cpp_info.components["vtx_python_bin"].requires =["vtx_util::vtx_util", "vtx_core::vtx_core", "vtx_app::vtx_app", "vtx_io::vtx_io", "pybind11::pybind11", "pybind11::embed"]

        self.cpp_info.components["vtx_python_binding_test"].libs = ["vtx_python_binding_test"] 
        self.cpp_info.components["vtx_python_binding_test"].requires =["vtx_util::vtx_util", "vtx_core::vtx_core", "vtx_app::vtx_app_no_opengl", "vtx_io::vtx_io", "pybind11::pybind11", "pybind11::embed", "vtx_python_bin"]
  
        filename = "*.pyd" if self.settings.os == "Windows" else "*.so"        
        path_python_module = os.path.join(self.package_folder, "**", filename)
        files = glob.glob(path_python_module, recursive=True)
        
        cmake_file_list = ["cmake/vtx_python_binding_copy_files.cmake"]
        
        if len(files) > 0:
            print("Found python module: " + files[0])
            self.conf_info.define("user.myconf:path_python_module", files[0])
            
            cmake_dir = os.path.join(self.recipe_folder, "cmake")
            if not Path(cmake_dir).exists():
                Path(cmake_dir).mkdir()
            cmake_dir = os.path.join(cmake_dir, "out")
            if not Path(cmake_dir).exists():
                Path(cmake_dir).mkdir()
            
            cmake_file_name = f"{self._generated_cmake_prefix()}{self.settings.build_type}.cmake"
            cmake_file_path = os.path.join(cmake_dir, cmake_file_name)
            
            cmake_file_content = """vtx_register_build_file_copy("%s" ".")""" % ((Path(files[0])).as_posix())
            Path(cmake_file_path).write_text(cmake_file_content)
            cmake_file_list.append(cmake_file_path)
            
        # Give away cmake code to be executed by the consumer of this package
        self.cpp_info.set_property("cmake_build_modules", cmake_file_list)
