#ifndef __VTX_PYTHON_BINDING_WRAPPER_MODULE__
#define __VTX_PYTHON_BINDING_WRAPPER_MODULE__

#include "python_binding/pybind11_extra_convertor.hpp"
#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <utility>

namespace VTX::PythonBinding
{
	class Interpretor;

	namespace Wrapper
	{
		class Module
		{
			friend PythonBinding::Interpretor;

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

		  private:
			Module( pybind11::module_ & p_module ) : _pyModule( p_module ) {}
			pybind11::module_ _pyModule;

			template<typename Action, typename... Args>
			static void runAction( Args... p_args )
			{
				Action action = Action( p_args... );
				action.execute();
			}
		};
	}; // namespace Wrapper
};	   // namespace VTX::PythonBinding
#endif
