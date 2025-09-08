conan export ../util
conan export ../core
conan export ../io
conan create . -b missing -s build_type=Debug -s compiler.cppstd=20 -o vtx_python_binding/*:test=True
