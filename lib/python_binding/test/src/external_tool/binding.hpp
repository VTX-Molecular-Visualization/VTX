#pragma once

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
