import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy
from pathlib import Path

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
        # Package options.
        self.options["qt"].shared = True
        self.options["qt"].opengl = "desktop"
        self.options["qt"].with_vulkan = False
        self.options["qt"].openssl = True
        self.options["qt"].with_pcre2 = True
        self.options["qt"].with_glib = False
        self.options["qt"].with_doubleconversion = True
        self.options["qt"].with_freetype = True
        self.options["qt"].with_fontconfig = False
        self.options["qt"].with_icu = False
        self.options["qt"].with_harfbuzz = False
        self.options["qt"].with_libjpeg = "libjpeg"
        self.options["qt"].with_libpng = True
        self.options["qt"].with_sqlite3 = False
        self.options["qt"].with_mysql = False
        self.options["qt"].with_pq = False
        self.options["qt"].with_odbc = False
        self.options["qt"].with_zstd = False
        self.options["qt"].with_brotli = False
        self.options["qt"].with_dbus = False
        self.options["qt"].with_libalsa = False
        self.options["qt"].with_openal = False
        self.options["qt"].with_gstreamer = False
        self.options["qt"].with_pulseaudio = False
        self.options["qt"].with_gssapi = False
        self.options["qt"].with_md4c = False
        self.options["qt"].with_x11 = False
        self.options["qt"].with_egl = False
        
        self.options["qt"].gui = True
        self.options["qt"].widgets = True
        
        self.options["qt"].device: None
        self.options["qt"].cross_compile: None
        self.options["qt"].sysroot: None
        #self.options["qt"].multiconfiguration: True
        self.options["qt"].disabled_features = ""
        
        # Qt modules.        
        self.options["qt"].essential_modules = False;
        self.options["qt"].addon_modules = False;
        self.options["qt"].deprecated_modules = False;
        self.options["qt"].preview_modules = False;
        
        self.options["qt"].qtsvg = False
        self.options["qt"].qtdeclarative = False
        self.options["qt"].qttools = False
        self.options["qt"].qttranslations = False
        self.options["qt"].qtdoc = False
        self.options["qt"].qtwayland = False
        self.options["qt"].qtquickcontrols2 = False
        self.options["qt"].qtquicktimeline = False
        self.options["qt"].qtquick3d = False
        self.options["qt"].qtshadertools = False
        self.options["qt"].qt5compat = False
        self.options["qt"].qtactiveqt = False
        self.options["qt"].qtcharts = False
        self.options["qt"].qtdatavis3d = False
        self.options["qt"].qtlottie = False
        self.options["qt"].qtscxml = False
        self.options["qt"].qtvirtualkeyboard = False
        self.options["qt"].qt3d = False
        self.options["qt"].qtimageformats = True
        self.options["qt"].qtnetworkauth = False
        self.options["qt"].qtcoap = False
        self.options["qt"].qtmqtt = False
        self.options["qt"].qtopcua = False
        self.options["qt"].qtmultimedia = False
        self.options["qt"].qtlocation = False
        self.options["qt"].qtsensors = False
        self.options["qt"].qtconnectivity = False
        self.options["qt"].qtserialbus = False
        self.options["qt"].qtserialport = False
        self.options["qt"].qtwebsockets = False
        self.options["qt"].qtwebchannel = False
        self.options["qt"].qtwebengine = False
        self.options["qt"].qtwebview = False
        self.options["qt"].qtremoteobjects = False
        self.options["qt"].qtpositioning = False
        self.options["qt"].qtlanguageserver = False
        self.options["qt"].qtspeech = False
        self.options["qt"].qthttpserver = False
        self.options["qt"].qtquick3dphysics = False
        self.options["qt"].qtgrpc = False
        self.options["qt"].qtquickeffectmaker = False
        self.options["qt"].qtgraphs = False

        self.options["cpython"].with_gdbm = False # Doesn't work on windows. I'm not sure what it does.
        self.options["cpython"].shared = True # False by default. If set to False, DLLs will be missing.


    def generate(self):
        tc = CMakeToolchain(self)        
        tc.generate()

        copy(self, "*sdl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))

        # Copy Qt plugins and DLLs to the build folder.
        qtBinDir = self.dependencies["qt"].cpp_info.bindir
        qtPluginsDir = os.path.join(self.dependencies["qt"].package_folder, "plugins")
        destDir = os.path.join(self.build_folder, self.cpp.build.libdirs[0])

        binFiles = [ "Qt6Core*.dll", "Qt6Gui*.dll", "Qt6Widgets*.dll" ]
        for file in binFiles:
            copy(self, file, qtBinDir, destDir)

        pluginsFolers = [ "imageformats", "platforms", "styles", "tls" ]
        for folder in pluginsFolers:
            copy(self, "*.dll", os.path.join(qtPluginsDir, folder), os.path.join(destDir, folder))
        
        copy(self, "*", os.path.join(self.dependencies["gromacs"].package_folder, "external"), os.path.join(self.build_folder, "external"))        
        copy(self, "*", os.path.join(self.dependencies["gromacs"].package_folder, "data", "tools","mdprep","gromacs","top"), os.path.join(self.build_folder, "data", "tools", "mdprep", "gromacs", "top" ))        

        self._doPythonCopies()
    
    def _doPythonCopies(self):
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
        

    def _create_python39_zip(self, python_lib_path, zip_destination):
        import zipfile
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
