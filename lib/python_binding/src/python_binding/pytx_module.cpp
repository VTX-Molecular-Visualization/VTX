#include "python_binding/pytx_module.hpp"
#include <app/vtx_app.hpp>
#include <pybind11/pybind11.h>

namespace VTX::PythonBinding
{
	PyTXModule::PyTXModule( Wrapper::Module & p_mainModule ) :
		_mainModule( p_mainModule ), _commandModule( p_mainModule.getSubmodule( "Command" ) )
	{
	}

	Wrapper::Module &		PyTXModule::commands() { return _commandModule; }
	const Wrapper::Module & PyTXModule::commands() const { return _commandModule; }

} // namespace VTX::PythonBinding
