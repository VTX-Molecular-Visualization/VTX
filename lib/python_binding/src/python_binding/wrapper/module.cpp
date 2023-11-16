#include "python_binding/wrapper/module.hpp"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <util/logger.hpp>

namespace VTX::PythonBinding::Wrapper
{
	Module Module::defSubmodule( const std::string & p_submoduleName, const std::string & p_submoduleDesc )
	{
		pybind11::module_ module	 = _pyModule.def_submodule( p_submoduleName.c_str(), p_submoduleDesc.c_str() );
		const std::string		  modulePath = _modulePath + "." + p_submoduleName;

		return Module( module, modulePath );
	}
	Module Module::getSubmodule( const std::string & p_submoduleName )
	{
		const std::string submodulePath = _modulePath + "." + std::string( p_submoduleName.c_str() );
		pybind11::module_ module		= _pyModule.import( submodulePath.c_str() );

		return Module( module, submodulePath );
	}

	void Module::displayInfo() const
	{
		try
		{
			pybind11::exec( "print( dir() )", _pyModule.attr( "__dict__" ) );
		}
		catch ( pybind11::error_already_set e )
		{
			VTX_ERROR( "Display module info fail : {}", e.what() );
		}
	}

	Module::~Module() = default;
} // namespace VTX::PythonBinding::Wrapper
