#ifndef __VTX_PYTHON_BINDING_WRAPPER_FUNCTION__
#define __VTX_PYTHON_BINDING_WRAPPER_FUNCTION__

#include <pybind11/pybind11.h>
#include <string>
#include <util/exceptions.hpp>

namespace VTX::PythonBinding::Wrapper
{
	class Object;
	class Module;

	/**
	 * @brief Class responsible for holding a python function reference and forwarding calls.
	 */
	class Function
	{
	  public:
		Function( const Module & p_module, const std::string & p_funcName );
		Function( const Object & p_object, const std::string & p_funcName );
		~Function() = default;

		template<typename... Args>
		void run( Args... p_args )
		{
			try
			{
				_returnObj = _funcHandle( p_args... );
			}
			catch ( const pybind11::error_already_set & e )
			{
				throw( PythonWrapperException(
					"Function " + _functionPath + " not found with given args (" + e.what() + ")."
				) );
			}
		}

		template<typename T>
		T getReturnValue()
		{
			if constexpr ( std::is_same_v<T, Object> )
			{
				return _getReturnValue();
			}
			else
			{
				return _getReturnValue<T>();
			}
		}

	  private:
		std::string _functionPath;

		pybind11::detail::str_attr_accessor _funcHandle;
		pybind11::object					_returnObj;

		pybind11::detail::str_attr_accessor _getFunctionAccessor(
			const Module &		p_module,
			const std::string & p_funcName
		) const;
		pybind11::detail::str_attr_accessor _getFunctionAccessor(
			const Object &		p_object,
			const std::string & p_funcName
		) const;

		template<typename T>
		T _getReturnValue()
		{
			try
			{
				T res = _returnObj.cast<T>();
				return res;
			}
			catch ( const pybind11::error_already_set & e )
			{
				throw( PythonWrapperException(
					"Unable to cast return value of function " + _functionPath + " (" + e.what() + ")."
				) );
			}
		}

		Object _getReturnValue();
	};
}; // namespace VTX::PythonBinding::Wrapper

#endif
