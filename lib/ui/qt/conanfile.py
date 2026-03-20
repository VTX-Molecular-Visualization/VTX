import os
from pathlib import Path
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.files import copy

_CONF_QT_RUNTIME_ROOT = "user.ui_qt:runtime_root"


def _cmake_path(path: str) -> str:
    return path.replace("\\", "/")

def config_options_qt(p_conanFile : ConanFile):
    # Package options.
    p_conanFile.options["qt"].shared = True
    p_conanFile.options["qt"].opengl = "no"
    p_conanFile.options["qt"].with_vulkan = False
    p_conanFile.options["qt"].openssl = True
    p_conanFile.options["qt"].with_pcre2 = True
    p_conanFile.options["qt"].with_glib = False
    p_conanFile.options["qt"].with_doubleconversion = True
    p_conanFile.options["qt"].with_freetype = True
    p_conanFile.options["qt"].with_fontconfig = False
    p_conanFile.options["qt"].with_icu = False
    p_conanFile.options["qt"].with_harfbuzz = False
    p_conanFile.options["qt"].with_libjpeg = "libjpeg"
    p_conanFile.options["qt"].with_libpng = True
    p_conanFile.options["qt"].with_sqlite3 = False
    p_conanFile.options["qt"].with_mysql = False
    p_conanFile.options["qt"].with_pq = False
    p_conanFile.options["qt"].with_odbc = False
    p_conanFile.options["qt"].with_zstd = False
    p_conanFile.options["qt"].with_brotli = False
    p_conanFile.options["qt"].with_dbus = False
    p_conanFile.options["qt"].with_libalsa = False
    p_conanFile.options["qt"].with_openal = False
    p_conanFile.options["qt"].with_gstreamer = False
    p_conanFile.options["qt"].with_pulseaudio = False
    p_conanFile.options["qt"].with_gssapi = False
    p_conanFile.options["qt"].with_md4c = False
    p_conanFile.options["qt"].with_x11 = False
    p_conanFile.options["qt"].with_egl = False

    p_conanFile.options["qt"].gui = True
    p_conanFile.options["qt"].widgets = True

    p_conanFile.options["qt"].device = None
    p_conanFile.options["qt"].cross_compile = None
    p_conanFile.options["qt"].sysroot = None
    p_conanFile.options["qt"].multiconfiguration = False
    p_conanFile.options["qt"].disabled_features = ""

    # Qt modules.        
    p_conanFile.options["qt"].essential_modules = False;
    p_conanFile.options["qt"].addon_modules = False;
    p_conanFile.options["qt"].deprecated_modules = False;
    p_conanFile.options["qt"].preview_modules = False;

    p_conanFile.options["qt"].qtsvg = False
    p_conanFile.options["qt"].qtdeclarative = False
    p_conanFile.options["qt"].qttools = False
    p_conanFile.options["qt"].qttranslations = False
    p_conanFile.options["qt"].qtdoc = False
    p_conanFile.options["qt"].qtwayland = False
    p_conanFile.options["qt"].qtquickcontrols2 = False
    p_conanFile.options["qt"].qtquicktimeline = False
    p_conanFile.options["qt"].qtquick3d = False
    p_conanFile.options["qt"].qtshadertools = False
    p_conanFile.options["qt"].qt5compat = False
    p_conanFile.options["qt"].qtactiveqt = False
    p_conanFile.options["qt"].qtcharts = False
    p_conanFile.options["qt"].qtdatavis3d = False
    p_conanFile.options["qt"].qtlottie = False
    p_conanFile.options["qt"].qtscxml = False
    p_conanFile.options["qt"].qtvirtualkeyboard = False
    p_conanFile.options["qt"].qt3d = False
    p_conanFile.options["qt"].qtimageformats = True
    p_conanFile.options["qt"].qtnetworkauth = False
    p_conanFile.options["qt"].qtcoap = False
    p_conanFile.options["qt"].qtmqtt = False
    p_conanFile.options["qt"].qtopcua = False
    p_conanFile.options["qt"].qtmultimedia = False
    p_conanFile.options["qt"].qtlocation = False
    p_conanFile.options["qt"].qtsensors = False
    p_conanFile.options["qt"].qtconnectivity = False
    p_conanFile.options["qt"].qtserialbus = False
    p_conanFile.options["qt"].qtserialport = False
    p_conanFile.options["qt"].qtwebsockets = False
    p_conanFile.options["qt"].qtwebchannel = False
    p_conanFile.options["qt"].qtwebengine = False
    p_conanFile.options["qt"].qtwebview = False
    p_conanFile.options["qt"].qtremoteobjects = False
    p_conanFile.options["qt"].qtpositioning = False
    p_conanFile.options["qt"].qtlanguageserver = False
    p_conanFile.options["qt"].qtspeech = False
    p_conanFile.options["qt"].qthttpserver = False
    p_conanFile.options["qt"].qtquick3dphysics = False
    p_conanFile.options["qt"].qtgrpc = False
    p_conanFile.options["qt"].qtquickeffectmaker = False
    p_conanFile.options["qt"].qtgraphs = False
    if p_conanFile.settings.os == "Linux":
            p_conanFile.options["qt"].opengl = "desktop" # forced by egl.
            p_conanFile.options["qt"].qtwayland = True
            p_conanFile.options["qt"].with_x11 = True
            p_conanFile.options["qt"].with_egl = True # forced by wayland.
            p_conanFile.options["qt"].with_dbus = True
        
        
def generate_qt(p_conanFile : ConanFile):

    # Copy Qt plugins and DLLs to the build folder.
    qtPackageDir = p_conanFile.dependencies["qt"].package_folder
    qtBinDir = os.path.join(qtPackageDir, "bin")
    qtLibDir = os.path.join(qtPackageDir, "lib")
    qtPluginsDir = os.path.join(p_conanFile.dependencies["qt"].package_folder, "plugins")
    destDir = os.path.join(p_conanFile.build_folder, p_conanFile.cpp.build.libdirs[0])

    if p_conanFile.settings.os == "Windows":
        binFiles = [ "Qt6Core*.dll", "Qt6Gui*.dll", "Qt6Widgets*.dll" ]
        for file in binFiles:
            p_conanFile.output.highlight(f"Copying {file} from Qt bin directory to {destDir}")
            copy(p_conanFile, file, qtBinDir, destDir)

        pluginsFolers = [ "imageformats", "platforms", "styles", "tls" ]
        for folder in pluginsFolers:
            p_conanFile.output.highlight(f"Copying *.dll from Qt {folder} directory to {destDir}/{folder}")
            copy(p_conanFile, "*.dll", os.path.join(qtPluginsDir, folder), os.path.join(destDir, folder))
    elif p_conanFile.settings.os == "Linux":
        p_conanFile.output.highlight(f"Copying Qt shared libraries from {qtLibDir} to {destDir}")
        copy(p_conanFile, "libQt6*.so*", qtLibDir, destDir)

        for folder in os.listdir(qtPluginsDir):
            source_dir = os.path.join(qtPluginsDir, folder)
            if os.path.isdir(source_dir):
                p_conanFile.output.highlight(f"Copying Qt plugins from {source_dir} to {destDir}/{folder}")
                copy(p_conanFile, "*.so*", source_dir, os.path.join(destDir, folder))


def qt_runtime_root(p_conanFile: ConanFile) -> str:
    package_root = getattr(p_conanFile, "package_folder", None)
    if package_root and (
        os.path.isdir(os.path.join(package_root, "platforms"))
        or os.path.exists(os.path.join(package_root, "Qt6Core.dll"))
        or os.path.exists(os.path.join(package_root, "libQt6Core.so"))
    ):
        return _cmake_path(package_root)

    build_root = Path(p_conanFile.recipe_folder) / p_conanFile.folders.build / p_conanFile.cpp.build.libdirs[0]
    return _cmake_path(str(build_root))


class VTXUiQtRecipe(ConanFile):
    name = "vtx_ui_qt"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    generators = "CMakeDeps"
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "asset/*"
        
    def requirements(self):
        self.requires("vtx_util/1.0")
        self.requires("vtx_app/1.0")
        self.requires("vtx_renderer/1.0")
        self.requires("vtx_core/1.0")
        self.requires("vtx_python_binding/1.0")
        self.requires("qt/6.10.1", transitive_headers=True)
        if self.settings.os == "Linux":
            self.requires("freetype/2.14.1", override=True)
            self.requires("libffi/3.4.8", override=True)
            self.requires("wayland/1.24.0", override=True)
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC            
        config_options_qt(self)
            
    def layout(self):
        cmake_layout(self)      

    def generate(self):    
        tc = CMakeToolchain(self)
        python_binding_conf = self.dependencies["vtx_python_binding"].conf_info
        tc.cache_variables["CPYTHON_VERSION_MAJOR"] = python_binding_conf.get("user.python_binding:cpython_version_major")
        tc.cache_variables["CPYTHON_VERSION_MINOR"] = python_binding_conf.get("user.python_binding:cpython_version_minor")
        tc.cache_variables["CPYTHON_VERSION_PATCH"] = python_binding_conf.get("user.python_binding:cpython_version_patch")
        tc.cache_variables["VTX_QT_RUNTIME_ROOT"] = _cmake_path(
            os.path.join(self.build_folder, self.cpp.build.libdirs[0])
        )
        tc.generate()
        generate_qt(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx_ui_qt"]
        self.cpp_info.bindirs = [""]
        self.cpp_info.set_property("cmake_build_modules", ["cmake/vtx_qt_configure.cmake", "cmake/vtx_qt_add_resources.cmake", "cmake/vtx_qt_copy_runtime.cmake"])
        self.conf_info.define(_CONF_QT_RUNTIME_ROOT, qt_runtime_root(self))
