#ifndef __VTX_PYTHON_BINDING_BINDING_MODULE__
#define __VTX_PYTHON_BINDING_BINDING_MODULE__

namespace VTX::PythonBinding
{

	// Return the name of the vtx python module
	const char * vtx_module_name();

	// Return the name of the vtx python module initialization script
	const char * vtx_initialization_script_name();

} // namespace VTX::PythonBinding

#endif
