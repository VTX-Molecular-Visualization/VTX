#ifndef VTX_APP_PYTHON_BINDING_RUN_SCRIPT_HPP
#define VTX_APP_PYTHON_BINDING_RUN_SCRIPT_HPP

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

#endif
