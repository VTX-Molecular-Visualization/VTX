@ECHO OFF
conan editable remove lib/app
conan editable remove lib/core
conan editable remove lib/io
conan editable remove lib/python_binding
conan editable remove lib/renderer
conan editable remove lib/ui/qt
conan editable remove lib/util
conan editable remove tool/example
conan editable remove tool/mdprep
PAUSE