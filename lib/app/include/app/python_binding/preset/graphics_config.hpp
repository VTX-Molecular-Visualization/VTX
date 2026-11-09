#ifndef VTX_APP_PYTHON_BINDING_PRESET_GRAPHICS_CONFIG_HPP
#define VTX_APP_PYTHON_BINDING_PRESET_GRAPHICS_CONFIG_HPP

#include "app/python_binding/module.hpp"
#include "app/python_binding/preset/types.hpp"
#include <pybind11/pybind11.h>

namespace VTX::App::PythonBinding::Preset
{
	void bindGraphicsConfig( Module &, pybind11::class_<GraphicsConfigHandle> & );
}

#endif
