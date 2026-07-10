#ifndef __VTX_APP_PYTHON_BINDING_MODULE__
#define __VTX_APP_PYTHON_BINDING_MODULE__

#include "app/action/action_manager.hpp"
#include <python_binding/wrapper/module.hpp>
#include <string>
#include <utility>

namespace VTX::App::PythonBinding
{
	/**
	 * @brief Encapsulate PythonBinding::Wrapper::Module to simplify action binding.
	 */
	class Module
	{
	  public:
		Module( VTX::PythonBinding::Wrapper::Module & p_module ) : _module( p_module ) {}

		template<typename Action, typename... ActionArgs, typename... Extras>
		void bindAction( const std::string & p_name, const std::string & p_desc = "", Extras &&... p_extras )
		{
			_module.bindAction<App::Action::QueueAction<Action>, ActionArgs...>(
				p_name, p_desc, std::forward<Extras>( p_extras )...
			);
		}

		template<typename Func>
		void def( const std::string & p_name, Func && p_function, const std::string & p_desc )
		{ _module.def( p_name, std::forward<Func>( p_function ), p_desc ); }

		template<typename Func, typename... Extra>
		void def( const std::string & p_name, Func && p_function, const std::string & p_desc, Extra &&... p_extra )
		{ _module.def( p_name, std::forward<Func>( p_function ), p_desc, std::forward<Extra>( p_extra )... ); }

		inline pybind11::module_ & pyModule() noexcept { return _module.pyModule(); }

		inline void getPythonModule( pybind11::module_ ** p_in ) noexcept { _module.getPythonModule( p_in ); }

	  private:
		VTX::PythonBinding::Wrapper::Module & _module;
	};

	template<typename Action, typename... Args>
	void executeAction( Args &&... p_args )
	{
		App::Action::QueueAction<Action> action;
		action.execute( std::forward<Args>( p_args )... );
	}
} // namespace VTX::App::PythonBinding

#endif
