from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps

class VTXUtilRecipe(ConanFile):
    name = "VTXUtil"
    version = "1.0"
    package_type = "library"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    exports_sources = "CMakeLists.txt", "src/*", "include/*"
    
    def requirements(self):
        self.requires("glm/0.9.9.8")
        self.requires("spdlog/1.12.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("magic_enum/0.9.3")
        self.requires("asio/1.28.1")
        self.requires("catch2/3.4.0")
        
    def build_requirements(self):
        self.tool_requires("cmake/3.27.7")
    
    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["VTXUtil"]

