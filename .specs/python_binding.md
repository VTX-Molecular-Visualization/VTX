# Python Binding library

A fully intergrated library that allows the user to script actions on VTX in the Python programming language. 

## Architecture

The module in `lib/python_binding` is only responsible for bridging pybind11 external library to the `lib/app` module. 
We aim to de-couple pybind11 from VTX using our python_binding library.
Using `lib/python_binding`, our app library at `lib/app` will bind VTX Action to a custom python command.
Anything related to user experience is located in the `lib/ui/qt` library.

### Python embedding

#### Goal

We want the user to use VTX scripting regardless of what python distribution is installed on their machine, or even with no other python installation at all (which is seldom among Windows users). 
We also want the user to be able to install their own python libraries using pip.

### Implementation

The CPython project will be used as a dependancy via conan. Its binaries (i.e. python executable and dlls) will be bundled alongside with VTX binaries. Thus, VTX will use pybind11 linked with its bundled python distribution to execute python scripts. 