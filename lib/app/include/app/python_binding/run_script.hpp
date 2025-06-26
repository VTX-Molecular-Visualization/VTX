#ifndef __VTX_PYTHON_BINDING_BINDING_BINDERS_RUN_SCRIPT__
#define __VTX_PYTHON_BINDING_BINDING_BINDERS_RUN_SCRIPT__

#include <python_binding/vtx_python_module.hpp>
#include <util/filesystem.hpp>

namespace VTX::App::PythonBinding
{

	class RunScript
	{
	  public:
		void bind( VTX::PythonBinding::PyTXModule & p_vtxModule );
	};

} // namespace VTX::App::PythonBinding

#endif
