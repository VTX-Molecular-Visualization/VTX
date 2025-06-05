@ECHO OFF
conan export lib/app
conan export lib/core
conan export lib/io
conan export lib/python_binding
conan export lib/renderer
conan export lib/ui/qt
conan export lib/util
conan export tool/example
conan export tool/mdprep
PAUSE