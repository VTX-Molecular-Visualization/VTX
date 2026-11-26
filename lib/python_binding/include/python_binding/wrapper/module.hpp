#pragma once

#include "python_binding/wrapper/function.hpp"
#include "python_binding/wrapper/object.hpp"
#include "python_binding/wrapper/pybind11_extra_convertor.hpp"
#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <util/exceptions.hpp>
#include <utility>
#include <vector>

namespace VTX::PythonBinding
{
	class Interpretor;

	namespace Wrapper
	{
		/**
		 * @brief Class providing usage and edition on a python module.
		 */
		class Module
		{
			friend PythonBinding::Interpretor;
			friend Function;

		  public:
			/**
			 * @brief Completion visitor function type.
			 */
			using CompletionVisitor
				= std::function<void( const std::string &, const std::string &, const std::string & )>;

			/**
			 * @brief Constructor.
			 */
			Module() = delete;
			~Module();

			/**
			 * @brief Define a submodule in the current module.
			 */
			Module defSubmodule( const std::string & p_submoduleName, const std::string & p_submoduleDesc );

			/**
			 * @brief Get a submodule from the current module.
			 */
			Module getSubmodule( const std::string & p_submoduleName );

			/**
			 * @brief Bind an action to the module.
			 */
			template<typename Action, typename... ActionArgs, typename... Extras>
			void bindAction( const std::string & p_name, const std::string & p_desc = "", Extras &&... p_extras )
			{
				const std::function<void( const ActionArgs &... )> runActionFunc = []( const ActionArgs &... p_args )
				{
					Action action;
					action.execute( p_args... );
				};
				def( p_name, runActionFunc, p_desc, std::forward<Extras>( p_extras )... );
			}

			/**
			 * @brief Define a function in the module.
			 */
			template<typename Func>
			void def( const std::string & p_name, Func && p_function, const std::string & p_desc )
			{ _pyModule.def( p_name.c_str(), std::forward<Func>( p_function ), p_desc.c_str() ); }

			template<typename Func, typename... Extra>
			void def( const std::string & p_name, Func && p_function, const std::string & p_desc, Extra &&... p_extra )
			{
				_pyModule.def(
					p_name.c_str(),
					std::forward<Func>( p_function ),
					p_desc.c_str(),
					convertToPybind11Extra( std::forward<Extra>( p_extra ) )...
				);
			}

			/**
			 * @brief Run a function from the module.
			 */
			void runFunction( const std::string & p_funcName ) const
			{
				Wrapper::Function funcWrapper = Wrapper::Function( *this, p_funcName );
				funcWrapper.run();
			}

			/**
			 * @brief Run a function from the module and return its result.
			 */
			template<typename T, typename... Args>
			T runFunction( const std::string & p_funcName, const Args &... p_args ) const
			{
				Wrapper::Function funcWrapper = Wrapper::Function( *this, p_funcName );
				funcWrapper.run( p_args... );

				if constexpr ( !std::is_void_v<T> )
				{
					return funcWrapper.getReturnValue<T>();
				}
			}

			/**
			 * @brief Display the module information in the python console.
			 */
			void displayInfo() const;

			/**
			 * @brief Get the module path.
			 */
			const std::string & getModulePath() const noexcept { return _modulePath; }

			/**
			 * @brief Get the underlying pybind11 module.
			 */
			pybind11::module_ & pyModule() noexcept { return _pyModule; }

			/**
			 * @brief Get the underlying pybind11 module.
			 */
			void getPythonModule( pybind11::module_ ** p_in ) noexcept { *p_in = &_pyModule; }

			/**
			 * @brief Visit the completion table of the module and call the visitor.
			 */
			static void visitCompletionTable( const CompletionVisitor & );

		  private:
			/**
			 * @brief Constructor.
			 */
			Module( pybind11::module_ & p_module, std::string p_modulePath ) :
				_pyModule( p_module ), _modulePath( std::move( p_modulePath ) )
			{
			}

			/**
			 * @brief Underlying pybind11 module.
			 */
			pybind11::module_ _pyModule;

			/**
			 * @brief Path of the module in the python package hierarchy.
			 */
			std::string _modulePath;
		};
	} // namespace Wrapper
}; // namespace VTX::PythonBinding
