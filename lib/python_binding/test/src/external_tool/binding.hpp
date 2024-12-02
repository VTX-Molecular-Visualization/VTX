#ifndef __VTX_PYTHON_BINDING_TEST_EXTERNAL_TOOL_BINDING__
#define __VTX_PYTHON_BINDING_TEST_EXTERNAL_TOOL_BINDING__

#include <python_binding/binder.hpp>
#include <python_binding/vtx_python_module.hpp>

namespace VTX::Test::ExternalTool
{
	class Binder : public PythonBinding::Binder
	{
	  public:
		void bind( PythonBinding::PyTXModule & p_vtxModule ) override;
	};
}; // namespace VTX::Test::ExternalTool

#endif
