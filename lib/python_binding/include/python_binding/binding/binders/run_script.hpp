#ifndef __VTX_PYTHON_BINDING_BINDING_BINDERS_RUN_SCRIPT__
#define __VTX_PYTHON_BINDING_BINDING_BINDERS_RUN_SCRIPT__

#include <python_binding/vtx_python_module.hpp>
#include <util/filesystem.hpp>

namespace VTX::PythonBinding::Binding::Binders
{

	class RunScript
	{
	  public:
		explicit RunScript( FilePath );
		void bind( PythonBinding::PyTXModule & p_vtxModule );
	};

} // namespace VTX::PythonBinding::Binding::Binders

#endif
