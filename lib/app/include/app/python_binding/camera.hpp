#pragma once

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class CameraBinder
	{
	  public:
		/**
		 * @brief Bind the camera module to the Python interpreter.
		 */
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding
