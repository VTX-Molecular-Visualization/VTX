#include "python_binding/wrapper/function.hpp"
#include "python_binding/wrapper/module.hpp"
#include "python_binding/wrapper/object.hpp"

namespace VTX::PythonBinding::Wrapper
{
	Function::Function( const Module & p_module, const std::string & p_funcName ) :
		_funcHandle( _getFunctionAccessor( p_module, p_funcName ) ),
		_functionPath( p_module.getModulePath() + "." + p_funcName )
	{
	}
	Function::Function( const Object & p_object, const std::string & p_funcName ) :
		_funcHandle( _getFunctionAccessor( p_object, p_funcName ) ),
		_functionPath( p_object.getObjectName() + "." + p_funcName )
	{
	}

	Object Function::_getReturnValue()
	{
		Object res = Object( _returnObj );
		return res;
	}

	pybind11::detail::str_attr_accessor Function::_getFunctionAccessor( const Module &		p_module,
																		const std::string & p_funcName ) const
	{
		const pybind11::detail::str_attr_accessor funcHandle = p_module._pyModule.attr( p_funcName.c_str() );

		try
		{
			funcHandle.ptr();
		}
		catch ( const std::exception e )
		{
			throw( PythonWrapperException( "Function " + _functionPath + " not found." ) );
		}

		return funcHandle;
	}
	pybind11::detail::str_attr_accessor Function::_getFunctionAccessor( const Object &		p_object,
																		const std::string & p_funcName ) const
	{
		const pybind11::detail::str_attr_accessor funcHandle = p_object._pyObject.attr( p_funcName.c_str() );

		try
		{
			funcHandle.ptr();
		}
		catch ( const std::exception e )
		{
			throw( PythonWrapperException( "Function " + _functionPath + " not found." ) );
		}

		return funcHandle;
	}
}; // namespace VTX::PythonBinding::Wrapper
