import os
import shutil
from pathlib import Path
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.cmake import CMakeToolchain
from conan.tools.env import VirtualRunEnv
from conan.tools.files import copy

_CONF_RUNTIME_ROOT = "user.tool_mdprep:runtime_root"


def _cmake_path(path: str) -> str:
    return path.replace("\\", "/")


def editable_runtime_root(p_conanFile: ConanFile) -> str:
    build_root = Path(p_conanFile.recipe_folder) / p_conanFile.folders.build
    if build_root.name == "build":
        return str(build_root / p_conanFile.settings.get_safe("build_type", default="Release"))
    return str(build_root)


def executable_folder(p_conanFile: ConanFile) -> str:
    return editable_runtime_root(p_conanFile)


def _packaged_runtime_root(p_conanFile: ConanFile) -> Path | None:
    package_root = getattr(p_conanFile, "package_folder", None)
    if not package_root:
        return None

    root = Path(package_root)
    if (root / "external" / "tools" / "mdprep" / "gromacs").is_dir():
        return root

    return None


def _flatten_gmx_config_subdir(dest_root: str) -> None:
    # MSVC multi-config generators stage the gmx binary under an extra <config>
    # subdirectory (Release/Debug/...); single-config generators (Linux) don't.
    # Flatten it so the packaged runtime layout is identical across platforms and
    # matches the path hardcoded in the tool (external/tools/mdprep/gromacs/gmx.exe).
    gromacs_bin_dir = Path(dest_root) / "external" / "tools" / "mdprep" / "gromacs"
    if not gromacs_bin_dir.is_dir():
        return
    for config in ("Release", "Debug", "RelWithDebInfo", "MinSizeRel"):
        config_dir = gromacs_bin_dir / config
        if not config_dir.is_dir():
            continue
        for entry in config_dir.iterdir():
            target = gromacs_bin_dir / entry.name
            if target.exists():
                if target.is_dir():
                    shutil.rmtree(target)
                else:
                    target.unlink()
            shutil.move(str(entry), str(target))
        config_dir.rmdir()


def _copy_gromacs_runtime(p_conanFile: ConanFile, dest_root: str) -> None:
    gromacs_root = p_conanFile.dependencies["gromacs"].package_folder
    copy(
        p_conanFile,
        "*",
        os.path.join(gromacs_root, "external"),
        os.path.join(dest_root, "external"),
    )
    copy(
        p_conanFile,
        "*",
        os.path.join(gromacs_root, "data", "tools", "mdprep", "gromacs", "top"),
        os.path.join(dest_root, "data", "tools", "mdprep", "gromacs", "top"),
    )
    _flatten_gmx_config_subdir(dest_root)


def do_gromacs_copies(p_conanFile: ConanFile) -> None:
    _copy_gromacs_runtime(p_conanFile, executable_folder(p_conanFile))


def runtime_root(p_conanFile: ConanFile) -> str:
    packaged_root = _packaged_runtime_root(p_conanFile)
    if packaged_root is not None:
        return _cmake_path(str(packaged_root))
    return _cmake_path(editable_runtime_root(p_conanFile))

class VTXToolMdprepRecipe(ConanFile):
    name = "vtx_tool_mdprep"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "test": [True, False]}
    default_options = {"shared": False, "fPIC": True, "test": False}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "test/*", "data/*", "asset/*"

     
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_ui_qt/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("re2/20240702")
        self.requires("gromacs/2026.1")
        self.requires("catch2/3.15.0")

    def generate(self):
        tc = CMakeToolchain(self)
        app_conf = self.dependencies["vtx_app"].conf_info
        python_binding_conf = self.dependencies["vtx_python_binding"].conf_info
        tc.cache_variables["CPYTHON_VERSION_MAJOR"] = python_binding_conf.get("user.python_binding:cpython_version_major")
        tc.cache_variables["CPYTHON_VERSION_MINOR"] = python_binding_conf.get("user.python_binding:cpython_version_minor")
        tc.cache_variables["CPYTHON_VERSION_PATCH"] = python_binding_conf.get("user.python_binding:cpython_version_patch")
        tc.cache_variables["VTX_PYTHON_BINDING_RUNTIME_ROOT"] = python_binding_conf.get("user.python_binding:runtime_root")
        qt_conf = self.dependencies["vtx_ui_qt"].conf_info
        tc.cache_variables["VTX_QT_RUNTIME_ROOT"] = qt_conf.get("user.ui_qt:runtime_root")
        tc.cache_variables["VTX_UI_QT"] = "1"
        tc.cache_variables["VTX_TOOL_MDPREP_RUNTIME_ROOT"] = _cmake_path(executable_folder(self))
        tc.cache_variables["VTX_RENDERER"] = app_conf.get("user.app:renderer")
        tc.cache_variables["VTX_PYTHON_BINDING"] = app_conf.get("user.app:python_binding")
        tc.generate()
        VirtualRunEnv(self).generate()
        do_gromacs_copies(self)
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        self.cpp.build.libdirs = self.cpp.build.libdirs
        self.cpp.source.includedirs = self.cpp.source.includedirs

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.test == True:
            # TEMP diagnostic: dump the build tree to locate the test executable.
            # Remove once the root cause is found.
            self._dump_tree()
            cmake.ctest([ "--output-on-failure", "-V"])

    def _dump_tree(self):
        # TEMP diagnostic helper. Prints the directory architecture of the relevant
        # roots, capped at 20 items per directory, and highlights the test executable.
        target = "vtx_tool_mdprep_test"
        targets = {target, target + ".exe"}

        def walk(path, indent="", max_items=20):
            try:
                entries = sorted(
                    os.scandir(path), key=lambda e: (not e.is_dir(), e.name.lower())
                )
            except OSError as exc:
                self.output.info(f"{indent}<cannot list: {exc}>")
                return
            for entry in entries[:max_items]:
                if entry.name == "data" and entry.is_dir():
                    self.output.info(f"Not going in {entry.name}/")
                    continue
                if entry.is_dir(follow_symlinks=False):
                    self.output.info(f"{indent}{entry.name}/")
                    walk(entry.path, indent + "    ", max_items)
                elif entry.name in targets:
                    self.output.info(f"{indent}>>>>> {entry.name}  <==== TEST EXECUTABLE")
                else:
                    self.output.info(f"{indent}{entry.name}")
            if len(entries) > max_items:
                self.output.info(f"{indent}... (+{len(entries) - max_items} more items)")

        roots = {
            "build_folder": self.build_folder,
            "source_folder": self.source_folder,
            "executable_folder()": executable_folder(self),
        }
        self.output.info("Starting points : " + str(roots))
        for label, root in roots.items():
            self.output.info(f"================ {label}: {root} ================")
            if root and os.path.isdir(root):
                walk(root)
            else:
                self.output.info(f"  <not a directory>")
        
    def package(self):
        cmake = CMake(self)
        cmake.install()
        _copy_gromacs_runtime(self, self.package_folder)

    def package_info(self):
        self.cpp_info.libs = ["vtx_tool_mdprep"]
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_tool_mdprep_copy_data.cmake"])
        self.conf_info.define(_CONF_RUNTIME_ROOT, runtime_root(self))
        
