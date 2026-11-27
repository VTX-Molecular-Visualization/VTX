#pragma once

#include "app/python_binding/preset/types.hpp"
#include <pybind11/pybind11.h>

namespace VTX::App::PythonBinding::Preset
{
	/**
	 * @brief Bind.
	 */
	void bindColorLayout( pybind11::class_<ColorLayoutHandle> & );
} // namespace VTX::App::PythonBinding::Preset
