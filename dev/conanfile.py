import os
from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy
from conan.tools.system.package_manager import Apt
from pathlib import Path
import importlib.util
import sys

sys.path.append(str(Path(__file__).resolve().parent.parent / "lib" / "python_binding"))
from python_version import DEFAULT_PYTHON_VERSION, config_options_cpython, configure_toolchain, configure_runtime_toolchain, get_python_version



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
app_module = import_module_from_file( Path("..") / "lib" / "app" / "conanfile.py" )

class VTXRecipe(ConanFile):
    name = "vtx"
    version = "1.0"
    package_type = "application"
    options = {
        "version": ["ANY"],
        "tool_example": [True, False],
        "tool_mdprep": [True, False],
        "tool_topology_editor": [True, False],
        "local_pdb100": [True, False],
        "ui_qt": [True, False],
        "renderer": [True, False],
        "python_binding": [True, False],
        "python_version": ["ANY"],
        "cuda_arch": ["ANY"],
    }
    default_options = {
        "version": "0.0.0",
        "tool_example": False,
        "tool_mdprep": False,
        "tool_topology_editor": True,
        "local_pdb100": False,
        "ui_qt": True,
        "renderer": True,
        "python_binding": True,
        "python_version": DEFAULT_PYTHON_VERSION,
        "cuda_arch": renderer_module.DEFAULT_CUDA_ARCH,
    }
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeDeps"
        
    def _python_version(self):
        return get_python_version(str(self.options.python_version))

    def validate(self):
        if not self.options.ui_qt and self.options.tool_example:
            raise ConanInvalidConfiguration("tool_example currently requires Qt UI. Disable tool_example or enable ui_qt.")
        if not self.options.ui_qt and self.options.tool_mdprep:
            raise ConanInvalidConfiguration("tool_mdprep currently requires Qt UI. Disable tool_mdprep or enable ui_qt.")
        if not self.options.ui_qt and self.options.tool_topology_editor:
            raise ConanInvalidConfiguration("tool_topology_editor currently requires Qt UI. Disable tool_topology_editor or enable ui_qt.")

    def requirements(self):
        self.requires("glm/1.0.1")
        self.requires("spdlog/1.17.0")
        self.requires("magic_enum/0.9.7")
        self.requires("nlohmann_json/3.12.0")
        self.requires("cpr/1.14.2")
        self.requires("catch2/3.15.0")
        self.requires("chemfiles/2026.02.5")
        self.requires("argparse/3.2")
        if self.options.renderer:
            self.requires("sdl/3.4.0")
            self.requires("imgui/1.92.7")
        self.requires("stb/cci.20240531")
        self.requires("entt/3.16.0")
        if self.options.python_binding:
            self.requires("pybind11/3.0.1")
        if self.options.ui_qt:
            self.requires("qt/6.11.1")
        if self.options.tool_mdprep:
            self.requires("gromacs/2026.0.1")
        self.requires("re2/20240702")
        if self.options.local_pdb100:
            self.requires("libarchive/3.7.9")
            self.requires("boost/1.87.0") # 1.88 version break process package on windows
        self.requires("platformfolders/4.3.0")
        if self.options.python_binding:
            self.requires("cpython/{}".format(str(self._python_version())))
            self.requires("mpdecimal/4.0.0", force=True)
        if self.settings.os == "Linux":
            self.requires("fontconfig/2.17.1", override=True)
            self.requires("xkbcommon/1.6.0", force=True)
            self.requires("libffi/3.4.8", force=True)
            self.requires("wayland/1.24.0", force=True)

    def system_requirements(self):
        if self.options.renderer:
            renderer_module.install_system_dependencies(self)
        if self.options.ui_qt:
            qt_module.install_system_dependencies(self)

    def config_options(self):
        if self.options.ui_qt:
            qt_module.config_options_qt(self)
        if self.options.python_binding:
            config_options_cpython(self)
        
    def generate(self):
        tc = CMakeToolchain(self)
        if self.options.python_binding:
            configure_toolchain(tc, self._python_version())
            configure_runtime_toolchain(tc, python_binding_module.runtime_root(self))
        
        versionMajor, versionMinor, versionPatch = map(int, str(self.options.version).split('.'))
        tc.cache_variables["VTX_VERSION_MAJOR"] = versionMajor
        tc.cache_variables["VTX_VERSION_MINOR"] = versionMinor
        tc.cache_variables["VTX_VERSION_PATCH"] = versionPatch 
        tc.cache_variables["VTX_TOOL_EXAMPLE"] = 1 if self.options.tool_example else 0
        tc.cache_variables["VTX_TOOL_MDPREP"] = 1 if self.options.tool_mdprep else 0
        tc.cache_variables["VTX_TOOL_TOPOLOGY_EDITOR"] = 1 if self.options.tool_topology_editor else 0
        tc.cache_variables["VTX_UI_QT"] = 1 if self.options.ui_qt else 0
        tc.cache_variables["VTX_RENDERER"] = 1 if self.options.renderer else 0
        tc.cache_variables["VTX_PYTHON_BINDING"] = 1 if self.options.python_binding else 0
        tc.cache_variables["VTX_VELOPACK_LIB"] = app_module.velopack_lib_name(
            str(self.settings.os), str(self.settings.arch)
        )
        if self.options.renderer:
            tc.cache_variables["VTX_CUDA_ARCH"] = self.options.cuda_arch
        tc.cache_variables["LOCAL_PDB100"] = 1 if self.options.local_pdb100 else 0
        if self.options.ui_qt:
            tc.cache_variables["VTX_QT_RUNTIME_ROOT"] = qt_module.qt_runtime_root(self)
        if self.options.tool_mdprep:
            tc.cache_variables["VTX_TOOL_MDPREP_RUNTIME_ROOT"] = mdprep_module.runtime_root(self)
        
        tc.generate()

        if self.options.renderer:
            copy(self, "*sdl3*", os.path.join(self.dependencies["imgui"].package_folder,
                "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))
            copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
                "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))

        if self.options.python_binding:
            python_binding_module.do_python_copies(self, self._python_version())
        if self.options.ui_qt:
            qt_module.generate_qt(self)
        if self.options.tool_mdprep:
            mdprep_module.do_gromacs_copies(self)

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
