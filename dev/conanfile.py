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
        self.requires("libarchive/3.7.9")
        self.requires("boost/1.87.0") # 1.88 version break process package on windows
        self.requires("platformfolders/4.3.0")
        self.requires("cpython/{}".format(str(python_binding_module.pythonVersion()))) # v >= 3.10 not working with msvc compiler so far

    def config_options(self):   
        qt_module.config_options_qt(self)
        python_binding_module.config_options_cpython(self)
        
        if self.settings.os == "Linux":
            self.options["qt"].qtwayland = True
            self.options["qt"].with_x11 = True
            self.options["qt"].with_egl = True
            self.options["qt"].with_dbus = True
        
    def generate(self):
        tc = CMakeToolchain(self)
        python_binding_module.configureToolChain(tc)
        
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

        # Copy Qt plugins and DLLs to the build folder.
        qtBinDir = self.dependencies["qt"].cpp_info.bindir
        qtPluginsDir = os.path.join(self.dependencies["qt"].package_folder, "plugins")
        destDir = os.path.join(self.build_folder, self.cpp.build.libdirs[0])

        binFiles = [ "Qt6Core*.dll", "Qt6Gui*.dll", "Qt6Widgets*.dll", "Qt6Test*.dll" ]
        for file in binFiles:
            copy(self, file, qtBinDir, destDir)

        qt_module.generate_qt(self)
        python_binding_module.doPythonCopies(self)
        mdprep_module.copy_gromacs_stuff(self)

        
        pluginsFolers = [ "imageformats", "platforms", "styles", "tls" ]
        for folder in pluginsFolers:
            copy(self, "*.dll", os.path.join(qtPluginsDir, folder), os.path.join(destDir, folder))
        
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
