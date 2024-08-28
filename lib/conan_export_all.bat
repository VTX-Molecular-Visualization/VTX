@ECHO OFF
conan export app
conan export core
conan export io
conan export python_binding
conan export renderer
conan export ui/qt
conan export util
PAUSE