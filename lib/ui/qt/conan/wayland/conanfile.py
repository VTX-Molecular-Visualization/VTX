from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.gnu import PkgConfig
from conan.tools.system import package_manager


class WaylandSystemConan(ConanFile):
    """
    System wayland libraries (same approach as ConanCenter's xorg/system).

    libwayland-client must be the one of the system: GPU EGL drivers (e.g. NVIDIA egl-wayland2) load it too, and the
    static copy that ConanCenter's wayland recipe embeds into Qt crashes at OpenGL context creation.
    Used through an override (see lib/ui/qt/conanfile.py). Recipes whose tool_requires follow the host version
    (xkbcommon) also get it in the build context, hence the wayland-scanner component.
    """

    name = "wayland"
    version = "system"
    package_type = "shared-library"
    description = "Wayland libraries provided by the system package manager."
    license = "MIT"
    settings = "os", "arch", "compiler", "build_type"

    _COMPONENTS = ["wayland-client", "wayland-server", "wayland-cursor", "wayland-egl", "wayland-scanner"]

    def validate(self):
        if self.settings.os not in ["Linux", "FreeBSD"]:
            raise ConanInvalidConfiguration("wayland/system supports only Linux and FreeBSD")

    def package_id(self):
        self.info.clear()

    def system_requirements(self):
        package_manager.Apt(self).install(["libwayland-dev"], update=True, check=True)
        package_manager.Dnf(self).install(["wayland-devel"], update=True, check=True)
        package_manager.Yum(self).install(["wayland-devel"], update=True, check=True)
        package_manager.Zypper(self).install(["wayland-devel"], update=True, check=True)
        package_manager.PacMan(self).install(["wayland"], update=True, check=True)

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.includedirs = []
        self.cpp_info.libdirs = []

        for name in self._COMPONENTS:
            pkg_config = PkgConfig(self, name)
            component = self.cpp_info.components[name]
            pkg_config.fill_cpp_info(component, is_system=True)
            component.version = pkg_config.version
            component.set_property("pkg_config_name", name)
            component.set_property("component_version", pkg_config.version)
            component.bindirs = []
            component.includedirs = []
            component.libdirs = []
            component.set_property(
                "pkg_config_custom_content",
                "\n".join(
                    f"{key}={value}"
                    for key, value in pkg_config.variables.items()
                    if key not in ["pcfiledir", "prefix", "includedir"]
                ),
            )

        # Real version in generated config/pc files instead of "system" (Qt does find_package(Wayland 1.15)).
        self.cpp_info.set_property("system_package_version", PkgConfig(self, "wayland-client").version)

        self.cpp_info.components["wayland-cursor"].requires = ["wayland-client"]
        self.cpp_info.components["wayland-egl"].requires = ["wayland-client"]
