import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy
from conan.tools.system.package_manager import Apt
from pathlib import Path
import importlib.util
import sys

sys.path.append(str(Path(__file__).resolve().parent.parent / "lib" / "python_binding"))
from python_version import config_options_cpython, configure_toolchain, get_python_version



def import_module_from_file(file_path):
    file_path = str(Path(__file__).resolve().parent / file_path)
    spec = importlib.util.spec_from_file_location("conanfile.py", file_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module
    
python_binding_module = import_module_from_file( Path("..") / "lib" / "python_binding" / "conanfile.py" )
qt_module = import_module_from_file( Path("..") / "lib" / "ui"/ "qt" / "conanfile.py" )
mdprep_module = import_module_from_file( Path("..") / "tool" / "mdprep"/ "conanfile.py" )
renderer_module = import_module_from_file( Path("..") / "lib" / "renderer" / "conanfile.py" )

class VTXRecipe(ConanFile):
    name = "vtx"
    version = "1.0"
    package_type = "application"
    options = {"version": ["ANY"], "tool_example": [True, False], "tool_mdprep": [True, False], "local_pdb100": [True, False] }
    default_options = {"version": "0.0.0", "tool_example": False, "tool_mdprep": True, "local_pdb100": False }
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeDeps"
        
    def requirements(self):
        self.requires("glm/1.0.1")
        self.requires("spdlog/1.17.0")
        self.requires("magic_enum/0.9.7")
        self.requires("nlohmann_json/3.12.0")
        self.requires("cpr/1.14.1")
        self.requires("catch2/3.13.0")
        self.requires("chemfiles/2026.02.1")
        self.requires("sdl/3.4.0")
        self.requires("imgui/1.92.5")
        self.requires("stb/cci.20240531")
        self.requires("entt/3.16.0")
        self.requires("pybind11/2.13.6")
        self.requires("qt/6.10.1")
        self.requires("gromacs/2026.0")
        self.requires("re2/20240702")
        if self.options.local_pdb100:
            self.requires("libarchive/3.7.9")
            self.requires("boost/1.87.0") # 1.88 version break process package on windows
        self.requires("platformfolders/4.3.0")
        self.requires("cpython/{}".format(str(get_python_version()))) # v >= 3.10 not working with msvc compiler so far
        if self.settings.os == "Linux":
            self.requires("xkbcommon/1.6.0", override=True)
            self.requires("libffi/3.4.8", override=True)
            self.requires("wayland/1.24.0", override=True)

    def system_requirements(self):
        renderer_module.install_system_dependencies(self)

    def config_options(self):
        qt_module.config_options_qt(self)
        config_options_cpython(self)
        
    def generate(self):
        tc = CMakeToolchain(self)
        configure_toolchain(tc, get_python_version())
        
        versionMajor, versionMinor, versionPatch = map(int, str(self.options.version).split('.'))
        tc.cache_variables["VTX_VERSION_MAJOR"] = versionMajor
        tc.cache_variables["VTX_VERSION_MINOR"] = versionMinor
        tc.cache_variables["VTX_VERSION_PATCH"] = versionPatch 
        tc.cache_variables["VTX_TOOL_EXAMPLE"] = 1 if self.options.tool_example else 0
        tc.cache_variables["VTX_TOOL_MDPREP"] = 1 if self.options.tool_mdprep else 0
        tc.cache_variables["LOCAL_PDB100"] = 1 if self.options.local_pdb100 else 0
        
        tc.generate()

        copy(self, "*sdl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))

        qt_module.generate_qt(self)
        python_binding_module.doPythonCopies(self)
        copy(self, "*", os.path.join(self.dependencies["gromacs"].package_folder, "external"), os.path.join(self.build_folder, "external"))        
        copy(self, "*", os.path.join(self.dependencies["gromacs"].package_folder, "data", "tools", "mdprep", "gromacs", "top"), os.path.join(self.build_folder, "data", "tools", "mdprep", "gromacs", "top" ))        

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
