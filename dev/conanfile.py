import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
from conan.tools.files import copy

class VTXRecipe(ConanFile):
    name = "vtx"
    version = "1.0"
    package_type = "application"
    
    settings = "os", "compiler", "build_type", "arch"
    
    generators = "CMakeDeps"
        
    def requirements(self):
        self.requires("glm/0.9.9.8")
        self.requires("spdlog/1.13.0")
        self.requires("nlohmann_json/3.11.3")
        self.requires("magic_enum/0.9.5")
        self.requires("cpr/1.10.5")
        self.requires("catch2/3.4.0")
        self.requires("chemfiles/0.10.4")
        self.requires("glad/0.1.36")
        self.requires("sdl/2.28.3")
        self.requires("imgui/1.90.1")
        self.requires("stb/cci.20220909")
        self.requires("entt/3.11.1")
        self.requires("pybind11/2.11.1")
        self.requires("qt/6.6.1")
        self.requires("eigen/3.4.0") 
        self.requires("vtx-gromacs/2024.0") 
        self.requires("fmt/10.2.1")

    def config_options(self):
        self.options["glad/*"].shared = False
        self.options["glad/*"].spec = "gl"
        self.options["glad/*"].gl_profile = "core"
        self.options["glad/*"].gl_version = 4.5
        self.options["glad/*"].extensions = ["GL_NVX_gpu_memory_info"]
        # Package options.
        self.options["qt/*"].shared = True
        self.options["qt/*"].opengl = "desktop"
        self.options["qt/*"].with_vulkan = False
        self.options["qt/*"].openssl = False
        self.options["qt/*"].with_pcre2 = True
        self.options["qt/*"].with_glib = False
        self.options["qt/*"].with_doubleconversion = True
        self.options["qt/*"].with_freetype = False
        self.options["qt/*"].with_fontconfig = False
        self.options["qt/*"].with_icu = False
        self.options["qt/*"].with_harfbuzz = False
        self.options["qt/*"].with_libjpeg = False
        self.options["qt/*"].with_libpng = False
        self.options["qt/*"].with_sqlite3 = False
        self.options["qt/*"].with_mysql = False
        self.options["qt/*"].with_pq = False
        self.options["qt/*"].with_odbc = False
        self.options["qt/*"].with_zstd = False
        self.options["qt/*"].with_brotli = False
        self.options["qt/*"].with_dbus = False
        self.options["qt/*"].with_libalsa = False
        self.options["qt/*"].with_openal = False
        self.options["qt/*"].with_gstreamer = False
        self.options["qt/*"].with_pulseaudio = False
        self.options["qt/*"].with_gssapi = False
        self.options["qt/*"].with_md4c = False
        self.options["qt/*"].with_x11 = False
        # Qt modules.
        self.options["qt/*"].qtsvg = False
        self.options["qt/*"].qtdeclarative = False
        self.options["qt/*"].qttools = False
        self.options["qt/*"].qttranslations = False
        self.options["qt/*"].qtdoc = False
        self.options["qt/*"].qtwayland = False
        self.options["qt/*"].qtquickcontrols2 = False
        self.options["qt/*"].qtquicktimeline = False
        self.options["qt/*"].qtquick3d = False
        self.options["qt/*"].qtshadertools = False
        self.options["qt/*"].qt5compat = False
        self.options["qt/*"].qtactiveqt = False
        self.options["qt/*"].qtcharts = False
        self.options["qt/*"].qtdatavis3d = False
        self.options["qt/*"].qtlottie = False
        self.options["qt/*"].qtscxml = False
        self.options["qt/*"].qtvirtualkeyboard = False
        self.options["qt/*"].qt3d = False
        self.options["qt/*"].qtimageformats = True
        self.options["qt/*"].qtnetworkauth = False
        self.options["qt/*"].qtcoap = False
        self.options["qt/*"].qtmqtt = False
        self.options["qt/*"].qtopcua = False
        self.options["qt/*"].qtmultimedia = False
        self.options["qt/*"].qtlocation = False
        self.options["qt/*"].qtsensors = False
        self.options["qt/*"].qtconnectivity = False
        self.options["qt/*"].qtserialbus = False
        self.options["qt/*"].qtserialport = False
        self.options["qt/*"].qtwebsockets = False
        self.options["qt/*"].qtwebchannel = False
        self.options["qt/*"].qtwebengine = False
        self.options["qt/*"].qtwebview = False
        self.options["qt/*"].qtremoteobjects = False
        self.options["qt/*"].qtpositioning = False
        self.options["qt/*"].qtlanguageserver = False
        self.options["qt/*"].qtspeech = False
        self.options["qt/*"].qthttpserver = False
        self.options["qt/*"].qtquick3dphysics = False
        self.options["qt/*"].qtgrpc = False
        self.options["qt/*"].qtquickeffectmaker = False
        self.options["qt/*"].qtgraphs = False

    def generate(self):
        tc = CMakeToolchain(self)        
        tc.generate()

        copy(self, "*sdl2*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "vendor/imgui"))

        copy(self, "*.dll", self.dependencies["qt"].cpp_info.bindir, os.path.join(self.build_folder, self.cpp.build.libdirs[0]))
        copy(self, "*.dll", os.path.join(self.dependencies["qt"].package_folder, "res/archdatadir/plugins"), os.path.join(self.build_folder, self.cpp.build.libdirs[0]))    

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vtx"]