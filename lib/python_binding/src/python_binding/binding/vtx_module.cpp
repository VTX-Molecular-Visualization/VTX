#include "python_binding/binding/vtx_module.hpp"
#include <pybind11/pybind11.h>

namespace VTX::PythonBinding
{
	const char * vtx_module_name() { return "vtx"; }

	const char * vtx_initialization_script_name() { return "vtx_module_init.py"; }

} // namespace VTX::PythonBinding
