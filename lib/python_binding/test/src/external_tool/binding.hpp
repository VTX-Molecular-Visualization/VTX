#ifndef VTX_PYTHON_BINDING_TEST_EXTERNAL_TOOL_BINDING_HPP
#define VTX_PYTHON_BINDING_TEST_EXTERNAL_TOOL_BINDING_HPP

#include <python_binding/binder.hpp>
#include <python_binding/wrapper/module.hpp>

namespace VTX::Test::ExternalTool
{

	class Binder
	{
	  public:
		void bind( PythonBinding::Wrapper::Module & p_vtxModule );
	};
}; // namespace VTX::Test::ExternalTool

#endif
