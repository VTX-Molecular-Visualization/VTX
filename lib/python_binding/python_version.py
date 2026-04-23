from dataclasses import dataclass


_CONF_MAJOR = "user.python_binding:cpython_version_major"
_CONF_MINOR = "user.python_binding:cpython_version_minor"
_CONF_PATCH = "user.python_binding:cpython_version_patch"
_CONF_RUNTIME_ROOT = "user.python_binding:runtime_root"

DEFAULT_PYTHON_VERSION = "3.12.7"


def _cmake_path(path: str | None) -> str | None:
    if path is None:
        return None
    return path.replace("\\", "/")


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


def parse_python_version(version: str) -> PythonVersion:
    major, minor, patch = version.split(".")
    return PythonVersion(major, minor, patch)


def get_python_version(version: str | None = None) -> PythonVersion:
    return parse_python_version(version or DEFAULT_PYTHON_VERSION)


def configure_toolchain(tc, version: PythonVersion | None = None) -> None:
    version = version or get_python_version()
    tc.cache_variables["CPYTHON_VERSION_MAJOR"] = version.major
    tc.cache_variables["CPYTHON_VERSION_MINOR"] = version.minor
    tc.cache_variables["CPYTHON_VERSION_PATCH"] = version.patch


def configure_toolchain_from_python_binding_dependency(tc, dependency) -> None:
    tc.cache_variables["CPYTHON_VERSION_MAJOR"] = dependency.conf_info.get(_CONF_MAJOR)
    tc.cache_variables["CPYTHON_VERSION_MINOR"] = dependency.conf_info.get(_CONF_MINOR)
    tc.cache_variables["CPYTHON_VERSION_PATCH"] = dependency.conf_info.get(_CONF_PATCH)
    tc.cache_variables["VTX_PYTHON_BINDING_RUNTIME_ROOT"] = dependency.conf_info.get(_CONF_RUNTIME_ROOT)


def configure_runtime_toolchain(tc, runtime_root: str) -> None:
    tc.cache_variables["VTX_PYTHON_BINDING_RUNTIME_ROOT"] = _cmake_path(runtime_root)


def config_options_cpython(conan_file) -> None:
    conan_file.options["cpython"].with_gdbm = False
    conan_file.options["cpython"].shared = conan_file.settings.os == "Windows"


def define_python_binding_conf(
    conf_info,
    version: PythonVersion | None = None,
    runtime_root: str | None = None,
) -> None:
    version = version or get_python_version()
    conf_info.define(_CONF_MAJOR, version.major)
    conf_info.define(_CONF_MINOR, version.minor)
    conf_info.define(_CONF_PATCH, version.patch)
    if runtime_root is not None:
        conf_info.define(_CONF_RUNTIME_ROOT, _cmake_path(runtime_root))
