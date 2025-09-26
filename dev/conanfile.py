import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy
from pathlib import Path
import importlib.util



def import_module_from_file(file_path):
    file_path = str(Path(__file__).resolve().parent / file_path)
    spec = importlib.util.spec_from_file_location("conanfile.py", file_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module
    

python_binding_module = import_module_from_file( Path("..") / "lib" / "python_binding" / "conanfile.py" )
qt_module = import_module_from_file( Path("..") / "lib" / "ui"/ "qt" / "conanfile.py" )
mdprep_module = import_module_from_file( Path("..") / "tool" / "mdprep"/ "conanfile.py" )

class VTXRecipe(ConanFile):
    name = "vtx"
    version = "1.0"
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeDeps"
        
    def requirements(self):
        self.requires("glm/1.0.1")
        self.requires("spdlog/1.15.3")
        self.requires("magic_enum/0.9.7")
        self.requires("nlohmann_json/3.12.0")
        self.requires("cpr/1.11.2")
        self.requires("catch2/3.8.1")
        self.requires("chemfiles/0.10.4.2")
        self.requires("sdl/3.2.18")
        self.requires("imgui/1.92.0")
        self.requires("stb/cci.20240531")
        self.requires("entt/3.15.0")
        self.requires("pybind11/2.13.6")
        self.requires("qt/6.8.3")
        self.requires("gromacs/2024.0")
        self.requires("re2/20240702")
        self.requires("libarchive/3.7.9")
        self.requires("boost/1.87.0") # 1.88 version break process package on windows
        self.requires("platformfolders/4.3.0")
        self.requires("cpython/3.9.19") # v >= 3.10 not working with msvc compiler so far

    def config_options(self):   
        qt_module.config_options_qt(self)
        python_binding_module.config_options_cpython(self)

    def generate(self):
        tc = CMakeToolchain(self)        
        tc.generate()

        copy(self, "*sdl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))


        qt_module.generate_qt(self)
        python_binding_module.doPythonCopies(self)
        mdprep_module.copy_gromacs_stuff(self)

        
    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        #cmake.configure(None, None, ["--graphviz=graph.dot"])
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx"]
