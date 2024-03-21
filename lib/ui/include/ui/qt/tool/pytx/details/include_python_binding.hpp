// Push "slots" macro declared in qt because used as member name in PyType_Spec in Python37/Python/include/object.h
// (also in other versions of Python ?)
#pragma push_macro( "slots" )
#undef slots
#include <python_binding/binder.hpp>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <python_binding/wrapper/module.hpp>
#pragma pop_macro( "slots" )
