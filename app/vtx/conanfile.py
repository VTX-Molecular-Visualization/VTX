import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.errors import ConanInvalidConfiguration

class VTXRecipe(ConanFile):
    name = "vtx"
    version = "1.0"
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "version": ["ANY"],
        "tool_example": [True, False],
        "tool_mdprep": [True, False],
        "tool_structalign": [True, False],
        "tool_topology_editor": [True, False],
        "ui_qt": [True, False],
        "renderer": [True, False],
        "python_binding": [True, False],
    }
    default_options = {
        "version": "0.0.0",
        "tool_example": False,
        "tool_mdprep": True,
        "tool_structalign": True,
        "tool_topology_editor": False,
        "ui_qt": True,
        "renderer": True,
        "python_binding": True,
    }

    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "asset/*", "default_user_data/*", "cmake/*", "license.txt"

    def configure(self):
        self.options["vtx_app"].renderer = self.options.renderer
        self.options["vtx_app"].python_binding = self.options.python_binding

    def validate(self):
        if not self.options.ui_qt and self.options.tool_example:
            raise ConanInvalidConfiguration("vtx_tool_example currently requires Qt UI. Disable tool_example or enable ui_qt.")
        if not self.options.ui_qt and self.options.tool_mdprep:
            raise ConanInvalidConfiguration("vtx_tool_mdprep currently requires Qt UI. Disable tool_mdprep or enable ui_qt.")
        if not self.options.ui_qt and self.options.tool_topology_editor:
            raise ConanInvalidConfiguration("vtx_tool_topology_editor currently requires Qt UI. Disable tool_topology_editor or enable ui_qt.")
    
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_core/1.0")
        if self.options.ui_qt:
            self.requires("vtx_ui_qt/1.0")
        if self.options.tool_example:
            self.requires("vtx_tool_example/1.0")
        if self.options.tool_mdprep:
            self.requires("vtx_tool_mdprep/1.0")
        if self.options.tool_structalign:
            self.requires("vtx_tool_structalign/1.0")
        if self.options.tool_topology_editor:
            self.requires("vtx_tool_topology_editor/1.0")

    def layout(self):
        cmake_layout(self)
        
    def generate(self):
        tc = CMakeToolchain(self)
        versionMajor, versionMinor, versionPatch = map(int, str(self.dependencies["vtx_app"].options.version).split('.'))
        app_conf = self.dependencies["vtx_app"].conf_info
        tc.cache_variables["VTX_VERSION_MAJOR"] = versionMajor
        tc.cache_variables["VTX_VERSION_MINOR"] = versionMinor
        tc.cache_variables["VTX_VERSION_PATCH"] = versionPatch 
        tc.cache_variables["VTX_TOOL_EXAMPLE"] = 1 if self.options.tool_example else 0
        tc.cache_variables["VTX_TOOL_MDPREP"] = 1 if self.options.tool_mdprep else 0
        tc.cache_variables["TOOL_STRUCTALIGN"] = 1 if self.options.tool_structalign else 0
        tc.cache_variables["VTX_TOOL_TOPOLOGY_EDITOR"] = 1 if self.options.tool_topology_editor else 0
        tc.cache_variables["VTX_UI_QT"] = 1 if self.options.ui_qt else 0
        tc.cache_variables["VTX_RENDERER"] = app_conf.get("user.app:renderer")
        tc.cache_variables["VTX_PYTHON_BINDING"] = app_conf.get("user.app:python_binding")
        if self.options.python_binding:
            python_binding_conf = self.dependencies["vtx_python_binding"].conf_info
            tc.cache_variables["VTX_PYTHON_BINDING_RUNTIME_ROOT"] = python_binding_conf.get("user.python_binding:runtime_root")
        if self.options.ui_qt:
            qt_conf = self.dependencies["vtx_ui_qt"].conf_info
            tc.cache_variables["VTX_QT_RUNTIME_ROOT"] = qt_conf.get("user.ui_qt:runtime_root")
        if self.options.tool_mdprep:
            mdprep_conf = self.dependencies["vtx_tool_mdprep"].conf_info
            tc.cache_variables["VTX_TOOL_MDPREP_RUNTIME_ROOT"] = mdprep_conf.get("user.tool_mdprep:runtime_root")
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx"]
