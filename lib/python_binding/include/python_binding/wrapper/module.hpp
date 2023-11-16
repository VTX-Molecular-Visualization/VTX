#ifndef __VTX_PYTHON_BINDING_WRAPPER_MODULE__
#define __VTX_PYTHON_BINDING_WRAPPER_MODULE__

#include "python_binding/wrapper/function.hpp"
#include "python_binding/wrapper/object.hpp"
#include "python_binding/wrapper/pybind11_extra_convertor.hpp"
#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <util/exceptions.hpp>

namespace VTX::PythonBinding
{
	class Interpretor;

	namespace Wrapper
	{
		class Module
		{
			friend PythonBinding::Interpretor;
			friend Function;

		  public:
			Module() = delete;
			~Module();

			Module defSubmodule( const std::string & p_submoduleName, const std::string & p_submoduleDesc );
			Module getSubmodule( const std::string & p_submoduleName );

			template<typename Action, typename... Args, typename... Extras>
			void bindAction( const std::string & p_name, const std::string & p_desc = "", Extras... p_extras )
			{
				std::function runActionFunc = &Module::runAction<Action, Args...>;
				def( p_name.c_str(), runActionFunc, p_desc.c_str(), p_extras... );
			}

			template<typename Func>
			void def( const std::string & p_name, Func p_function, const std::string & p_desc )
			{
				_pyModule.def( p_name.c_str(), p_function, p_desc.c_str() );
			}
			template<typename Func, typename... Extra>
			void def( const std::string & p_name, Func p_function, const std::string & p_desc, Extra... p_extra )
			{
				_pyModule.def( p_name.c_str(),
							   p_function,
							   p_desc.c_str(),
							   Pybind11ExtraConvertor::convertToPybind11Extra( std::forward<Extra>( p_extra ) )... );
			}

			template<typename T>
			T runFunction( const std::string & p_funcName ) const
			{
				Wrapper::Function funcWrapper = Wrapper::Function( *this, p_funcName );
				funcWrapper.run();

				return funcWrapper.getReturnValue<T>();
			}
			template<>
			void runFunction( const std::string & p_funcName ) const
			{
				Wrapper::Function funcWrapper = Wrapper::Function( *this, p_funcName );
				funcWrapper.run();
			}
			template<typename T, typename... Args>
			T runFunction( const std::string & p_funcName, Args... p_args ) const
			{
				Wrapper::Function funcWrapper = Wrapper::Function( *this, p_funcName );
				funcWrapper.run( p_args... );

				return funcWrapper.getReturnValue<T>();
			}
			template<typename... Args>
			void runFunction( const std::string & p_funcName, Args... p_args ) const
			{
				Wrapper::Function funcWrapper = Wrapper::Function( *this, p_funcName );
				funcWrapper.run( p_args... );
			}

			void				displayInfo() const;
			const std::string & getModulePath() const { return _modulePath; }

		  private:
			Module( pybind11::module_ & p_module, const std::string & p_modulePath ) :
				_pyModule( p_module ), _modulePath( p_modulePath )
			{
			}
			pybind11::module_ _pyModule;
			std::string		  _modulePath;

			template<typename Action, typename... Args>
			static void runAction( Args... p_args )
			{
				Action action = Action( p_args... );
				action.execute();
			}
		};
	} // namespace Wrapper
};	  // namespace VTX::PythonBinding

#endif
