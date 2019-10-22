<!--- Batch image URLs generated at https://shields.io -->
[![Release](https://img.shields.io/github/release/rcsb/mmtf-cpp.svg?style=flat)](https://github.com/rcsb/mmtf-cpp/releases)
[![License](https://img.shields.io/github/license/rcsb/mmtf-cpp.svg?style=flat)](https://github.com/rcsb/mmtf-cpp/blob/master/LICENSE)
[![Build Status (Travis)](https://img.shields.io/travis/rcsb/mmtf-cpp/master.svg?style=flat)](https://travis-ci.org/rcsb/mmtf-cpp)
[![Build Status (AppVeyor)](https://img.shields.io/appveyor/ci/rcsb/mmtf-cpp/master.svg?style=flat)](https://ci.appveyor.com/project/rcsb/mmtf-cpp)

The <b>m</b>acro<b>m</b>olecular <b>t</b>ransmission <b>f</b>ormat
([MMTF](http://mmtf.rcsb.org)) is a binary encoding of biological structures.

This repository holds the C++-03 compatible API, encoding and decoding
libraries. The MMTF specification can be found
[here](https://github.com/rcsb/mmtf/blob/HEAD/spec.md/).

## Prerequisites for using MMTF in C++

You need the headers of the MessagePack C++ library (version 2.1.5 or newer).
If you do not have them on your machine already, you can download the "include"
directory from the
[MessagePack GitHub repository](https://github.com/msgpack/msgpack-c).

## How to use MMTF

You only need to include the mmtf.hpp header in your code to use MMTF.
For instance a minimal example to load an MMTF file looks as follows:

```C
#include <mmtf.hpp>

int main(int argc, char** argv) {
    mmtf::StructureData data;
    mmtf::decodeFromFile(data, "test.mmtf");
    return 0;
}
```

The C++ MMTF library is header only so you do not need to compile it. If you
have a source file named `demo.cpp` (e.g. including the code above), you can
generate an executable `demo.exe` as follows:

```bash
g++ -I<MSGPACK_INCLUDE_PATH> -I<MMTF_INCLUDE_PATH> demo.cpp -o demo.exe
```

Here, `<MSGPACK_INCLUDE_PATH>` and `<MMTF_INCLUDE_PATH>` are the paths to the
"include" directories of MessagePack and this library respectively.

For your more complicated projects, a `CMakeLists.txt` is included for you.

## Installation
You can also perform a system wide installation with `cmake` and `ninja` (or `make`).  
To do so:
```bash
mkdir build
cd build
cmake -G Ninja ..
sudo ninja install
```

`cmake` automatically sets the installation directory to `/usr/local/include`, if you want to install it to another `*include/` directory
run `cmake` with the command:
```bash
cmake -G Ninja -DCMAKE_INSTALL_PREFIX=/home/me/local ..
```
Be aware that `/include` is added to the end of `DCMAKE_INSTALL_PREFIX` and that is where your files are installed (i.e. the above would install at `/home/me/local/include/`).


## Examples and tests

To build the tests + examples we recommend using the following lines:

```bash
# download Catch2 testing framework, msgpack-c, and the mmtf-spec test-dataset
git submodule update --init --recursive
mkdir build
cd build
cmake -G Ninja -DBUILD_TESTS=ON -Dmmtf_build_local=ON -Dmmtf_build_examples=ON ..
ninja
chmod +x ./tests/mmtf_tests
./tests/mmtf_tests
```

Example codes:
- mmtf_demo.cpp: Loads an MMTF file and checks internal consistency using
            mmtf::StructureData::hasConsistentData.
```bash
./examples/mmtf_demo ../mmtf_spec/test-suite/mmtf/173D.mmtf
```
- traverse.cpp: Loads an MMTF file and dumps it in human-readable forms.
```bash
./examples/traverse ../mmtf_spec/test-suite/mmtf/173D.mmtf
./examples/traverse ../mmtf_spec/test-suite/mmtf/173D.mmtf json
./examples/traverse ../mmtf_spec/test-suite/mmtf/173D.mmtf print
```

- print_as_pdb.cpp: Loads an MMTF file and prints it in pdb format.
```bash
./examples/print_as_pdb ../mmtf_spec/test-suite/mmtf/173D.mmtf
```

## Code documentation

You can generate a [doxygen](http://www.doxygen.org) based documentation of the
library by calling `doxygen` in the docs folder. You will need doxygen 1.8.11 or
later for that. Open `docs/html/index.html` to see the generated documentation.
