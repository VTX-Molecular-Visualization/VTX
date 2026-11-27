#pragma once

#include "app/python_binding/module.hpp"
#include "app/python_binding/preset/types.hpp"
#include <pybind11/pybind11.h>

namespace VTX::App::PythonBinding::Preset
{
	/**
	 * @brief Bind.
	 */
	void bindColorLayout( Module &, pybind11::class_<ColorLayoutHandle> & );
} // namespace VTX::App::PythonBinding::Preset
