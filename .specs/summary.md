# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System and Commands

**VTX uses Conan for dependency management and CMake for builds.**

### Essential Build Commands

```bash
# Navigate to dev directory first
cd dev

# Install dependencies and configure (from dev/ directory)
conan install . --build=missing

# Build the project in Release mode 
conan build . -b missing -s compiler.cppstd=20 -s build_type=Release
# or for debug build
conan build . -b missing -s compiler.cppstd=20 -s build_type=Debug
```

### Development Commands

```bash
# Export all conan packages (run from project root)
_conan_export_all.bat

# Clean build directories
_remove_build_folders.bat

# Format code (uses .clang-format configuration)
clang-format -i **/*.cpp **/*.hpp
```

### Testing

The project includes benchmarking applications in `app/bench/` and test projects in individual library directories under `lib/*/test/`. Run tests using CTest after building.

## Architecture Overview

**VTX is a modular C++ application for molecular visualization and molecular modelling.**

### Core Module Structure

- **`lib/core/`** - Core data structures, entities, and fundamental algorithms
- **`lib/io/`** - File I/O handling for molecular formats (PDB, etc.)
- **`lib/renderer/`** - Graphics rendering pipeline (OpenGL/CUDA-accelerated)
- **`lib/app/`** - Application logic and action controllers
- **`lib/ui/qt/`** - Qt-based user interface components
- **`lib/util/`** - Utility functions and common helpers
- **`lib/python_binding/`** - Python integration via pybind11

### Applications

- **`app/vtx/`** - Main VTX molecular visualization application
- **`app/bench/`** - Performance benchmarking tools

### Tools

- **`tool/example/`** - Example applications demonstrating library usage
- **`tool/mdprep/`** - Molecular dynamics preparation utilities

### Key Dependencies

The project uses Conan to manage dependencies including:
- **Qt 6.8.3** - GUI framework
- **OpenGL** - Graphics rendering
- **Python 3.9.19** - Scripting support
- **Catch2** - Testing framework
- **GROMACS** - Molecular dynamics integration
- **CUDA** - GPU acceleration (optional)

### Supported Plateforms

VTX supports the following plateforms :
- Windows >=10
- Ubuntu >=22.03

### Build Configuration

- **CMake presets** are defined in `dev/CMakePresets.json`
- **Conan configuration** in `dev/conanfile.py` defines all dependencies
- **Library linking** follows a modular pattern where each library can be built independently
- **Code formatting** uses Allman brace style with 4-space tabs (see `.clang-format`)

### Development Workflow

1. Make changes to library code in `lib/*/`
2. Update conanfile.py in both `lib/*/` and `dev/` if adding new dependencies
3. Build and test from `dev/` directory
4. The main executable links all required libraries statically

The project supports both Debug and Release configurations through CMake, with automatic dependency resolution via Conan.