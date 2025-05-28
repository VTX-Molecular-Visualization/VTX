#include "python_binding/wrapper/module.hpp"
#include "python_binding/binding/vtx_module.hpp"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <util/logger.hpp>

namespace VTX::PythonBinding::Wrapper
{
	Module Module::defSubmodule( const std::string & p_submoduleName, const std::string & p_submoduleDesc )
	{
		pybind11::module_ module	 = _pyModule.def_submodule( p_submoduleName.c_str(), p_submoduleDesc.c_str() );
		const std::string modulePath = _modulePath + "." + p_submoduleName;

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
		catch ( const pybind11::error_already_set & e )
		{
			VTX_ERROR( "Display module info fail : {}", e.what() );
		}
	}

	std::vector<std::string> Module::getFunctionList() const
	{
		std::vector<std::string> res = std::vector<std::string>();

		try
		{
			pybind11::list pyList
				= pybind11::eval( fmt::format( "dir({}.Command)", vtx_module_name() ) ).cast<pybind11::list>();

			res.reserve( pyList.size() );

			for ( const auto & value : pyList )
			{
				res.emplace_back( value.cast<std::string>() );
			}
			pyList = pybind11::eval( fmt::format( "dir({}.API)", vtx_module_name() ) ).cast<pybind11::list>();

			res.reserve( res.size() + pyList.size() );

			for ( const auto & value : pyList )
			{
				res.emplace_back( value.cast<std::string>() );
			}
		}
		catch ( const pybind11::error_already_set & e )
		{
			VTX_ERROR( "getFunctionList fail : {}", e.what() );
		}

		return res;
	}

	Module::~Module() = default;
} // namespace VTX::PythonBinding::Wrapper
