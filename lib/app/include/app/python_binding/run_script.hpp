#pragma once

#include <python_binding/wrapper/module.hpp>
#include <util/filesystem.hpp>

namespace VTX::App::PythonBinding
{

	class RunScript
	{
	  public:
		void bind( VTX::PythonBinding::Wrapper::Module & p_vtxModule );
	};

} // namespace VTX::App::PythonBinding
