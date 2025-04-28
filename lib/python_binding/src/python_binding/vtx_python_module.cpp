#include "python_binding/vtx_python_module.hpp"
#include <pybind11/pybind11.h>

namespace VTX::PythonBinding
{
	PyTXModule::PyTXModule( Wrapper::Module p_mainModule ) :
		_mainModule( std::move( p_mainModule ) ), _coreModule( p_mainModule.getSubmodule( "Core" ) ),
		_commandModule( p_mainModule.getSubmodule( "Command" ) ), _apiModule( p_mainModule.getSubmodule( "API" ) )
	{
	}

	const Wrapper::Module & PyTXModule::core() const { return _coreModule; }
	Wrapper::Module &		PyTXModule::core() { return _coreModule; }

	const Wrapper::Module & PyTXModule::commands() const { return _commandModule; }
	Wrapper::Module &		PyTXModule::commands() { return _commandModule; }

	const Wrapper::Module & PyTXModule::api() const { return _apiModule; }
	Wrapper::Module &		PyTXModule::api() { return _apiModule; }

} // namespace VTX::PythonBinding
