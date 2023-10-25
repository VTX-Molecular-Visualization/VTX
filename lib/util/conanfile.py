from conan import ConanFile

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("glm/0.9.9.8")
        self.requires("spdlog/1.12.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("magic_enum/0.9.3")
        self.requires("asio/1.28.1")
        self.requires("catch2/3.4.0")

    #def build_requirements(self):
    #    self.tool_requires("cmake/3.27.7")