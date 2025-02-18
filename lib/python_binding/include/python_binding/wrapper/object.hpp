#ifndef __VTX_PYTHON_BINDING_WRAPPER_OBJECT__
#define __VTX_PYTHON_BINDING_WRAPPER_OBJECT__

#include "python_binding/wrapper/function.hpp"
#include "python_binding/wrapper/pybind11_extra_convertor.hpp"
#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <util/exceptions.hpp>
#include <utility>

namespace VTX::PythonBinding
{
	namespace Wrapper
	{
		class Module;

		/**
		 * @brief Class responsible for providing read/write access to a generic python object
		 */
		class Object
		{
			friend Module;
			friend Function;

		  public:
			Object() = delete;
			~Object();

			template<typename... Args>
			void call( const std::string & p_function, Args... p_args )
			{
				Function func = Function( _pyObject, p_function );
				func.run( p_args... );
			}
			template<typename T, typename... Args>
			T call( const std::string & p_function, Args... p_args )
			{
				Function func = Function( _pyObject, p_function );
				func.run( p_args... );

				return func.getReturnValue<T>();
			}

			template<typename T>
			T getMember( const std::string & p_memberName )
			{
				return _pyObject.attr( p_memberName.c_str() ).cast<T>();
			}

			template<typename T, std::enable_if<std::is_same_v<T, Object>>>
			void setMember( const std::string & p_memberName, const Object & p_value )
			{
				try
				{
					return _pyObject.attr( p_memberName.c_str() ) = p_value._pyObject;
				}
				catch ( const pybind11::error_already_set & e )
				{
					throw PythonWrapperException(
						"Error when setting member " + std::string( pybind11::str( _pyObject ) ) + "::" + p_memberName
						+ " - " + e.what()
					);
				}
			}
			template<typename T>
			void setMember( const std::string & p_memberName, const T & p_value )
			{
				try
				{
					return _pyObject.attr( p_memberName.c_str() ) = p_value;
				}
				catch ( const pybind11::error_already_set & e )
				{
					throw PythonWrapperException(
						"Error when setting member " + std::string( pybind11::str( _pyObject ) ) + "::" + p_memberName
						+ " - " + e.what()
					);
				}
			}

			std::string getObjectName() const;

		  private:
			Object( pybind11::object & p_object ) : _pyObject( p_object ) {}
			pybind11::object _pyObject;
		};
	}; // namespace Wrapper
}; // namespace VTX::PythonBinding
#endif
