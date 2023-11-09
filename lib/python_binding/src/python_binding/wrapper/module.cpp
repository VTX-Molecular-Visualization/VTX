#include "python_binding/wrapper/module.hpp"

namespace VTX::PythonBinding::Wrapper
{
	Module Module::defSubmodule( const std::string & p_submoduleName, const std::string & p_submoduleDesc )
	{
		pybind11::module_ module = _pyModule.def_submodule( p_submoduleName.c_str(), p_submoduleDesc.c_str() );
		return Module( module );
	}
	Module Module::getSubmodule( const std::string & p_submoduleName )
	{
		const std::string submoduleCommand = "PyTX." + std::string( p_submoduleName.c_str() );

		pybind11::module_ module = _pyModule.import( submoduleCommand.c_str() );
		return Module( module );
	}

	Module::~Module() = default;
} // namespace VTX::PythonBinding::Wrapper
