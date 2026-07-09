#!/bin/bash
cd "$(dirname "$0")/.."
conan editable add lib/app
conan editable add lib/core
conan editable add lib/io
conan editable add lib/python_binding
conan editable add lib/renderer
conan editable add lib/ui/qt
conan editable add lib/util
conan editable add tool/example
conan editable add tool/mdprep
conan editable add tool/topology_editor
