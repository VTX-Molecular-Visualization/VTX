from dataclasses import dataclass


_CONF_MAJOR = "user.python_binding:cpython_version_major"
_CONF_MINOR = "user.python_binding:cpython_version_minor"
_CONF_PATCH = "user.python_binding:cpython_version_patch"


@dataclass(frozen=True)
class PythonVersion:
    major: str
    minor: str
    patch: str

    def __str__(self) -> str:
        return f"{self.major}.{self.minor}.{self.patch}"

    @property
    def short(self) -> str:
        return f"{self.major}.{self.minor}"

    @property
    def zip_name(self) -> str:
        return f"python{self.major}{self.minor}.zip"


def get_python_version() -> PythonVersion:
    return PythonVersion("3", "12", "7")


def configure_toolchain(tc, version: PythonVersion | None = None) -> None:
    version = version or get_python_version()
    tc.cache_variables["CPYTHON_VERSION_MAJOR"] = version.major
    tc.cache_variables["CPYTHON_VERSION_MINOR"] = version.minor
    tc.cache_variables["CPYTHON_VERSION_PATCH"] = version.patch


def configure_toolchain_from_python_binding_dependency(tc, dependency) -> None:
    tc.cache_variables["CPYTHON_VERSION_MAJOR"] = dependency.conf_info.get(_CONF_MAJOR)
    tc.cache_variables["CPYTHON_VERSION_MINOR"] = dependency.conf_info.get(_CONF_MINOR)
    tc.cache_variables["CPYTHON_VERSION_PATCH"] = dependency.conf_info.get(_CONF_PATCH)


def config_options_cpython(conan_file) -> None:
    conan_file.options["cpython"].with_gdbm = False
    conan_file.options["cpython"].shared = conan_file.settings.os == "Windows"


def define_python_binding_conf(conf_info, version: PythonVersion | None = None) -> None:
    version = version or get_python_version()
    conf_info.define(_CONF_MAJOR, version.major)
    conf_info.define(_CONF_MINOR, version.minor)
    conf_info.define(_CONF_PATCH, version.patch)
