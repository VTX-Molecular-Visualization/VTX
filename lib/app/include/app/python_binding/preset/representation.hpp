#ifndef VTX_APP_PYTHON_BINDING_PRESET_REPRESENTATION_HPP
#define VTX_APP_PYTHON_BINDING_PRESET_REPRESENTATION_HPP

#include "app/python_binding/module.hpp"
#include "app/python_binding/preset/types.hpp"
#include <pybind11/pybind11.h>

namespace VTX::App::PythonBinding::Preset
{
	void bindRepresentation( Module &, pybind11::class_<RepresentationHandle> & );
}

#endif
