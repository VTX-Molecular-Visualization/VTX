#ifndef VTX_APP_PYTHON_BINDING_MODULE_HPP
#define VTX_APP_PYTHON_BINDING_MODULE_HPP

#include "app/ecs.hpp"
#include "app/services.hpp"
#include <functional>
#include <pybind11/pybind11.h>
#include <python_binding/wrapper/module.hpp>
#include <string>
#include <type_traits>
#include <util/action_manager.hpp>
#include <util/thread/thread_manager.hpp>
#include <utility>

namespace VTX::App::PythonBinding
{
	/**
	 * @brief Execute action from python on the main thread (blocking).
	 */
	template<typename Action, typename... Args>
	void executeAction( Args &&... p_args )
	{
		THREAD().synchronize(
			[ & ]()
			{
				Action action;
				Util::ActionManager::run( action, std::forward<Args>( p_args )... );
			}
		);
	}

	/**
	 * @brief Action executed on the main thread.
	 */
	template<typename Action>
	struct MainThreadAction
	{
		template<typename... Args>
		void execute( Args &&... p_args )
		{ executeAction<Action>( std::forward<Args>( p_args )... ); }
	};

	/**
	 * @brief Encapsulate PythonBinding::Wrapper::Module to simplify action binding.
	 */
	class Module
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Module( VTX::PythonBinding::Wrapper::Module & p_module ) : _module( p_module ) {}

		/**
		 * @brief Bind an action.
		 */
		template<typename Action, typename... ActionArgs, typename... Extras>
		void bindAction( const std::string & p_name, const std::string & p_desc = "", Extras &&... p_extras )
		{
			_module.bindAction<MainThreadAction<Action>, ActionArgs...>(
				p_name, p_desc, std::forward<Extras>( p_extras )...
			);
		}

		/**
		 * @brief Def.
		 */
		template<typename Func>
		void def( const std::string & p_name, Func && p_function, const std::string & p_desc )
		{ _module.def( p_name, std::forward<Func>( p_function ), p_desc ); }

		template<typename Func, typename... Extra>
		void def( const std::string & p_name, Func && p_function, const std::string & p_desc, Extra &&... p_extra )
		{ _module.def( p_name, std::forward<Func>( p_function ), p_desc, std::forward<Extra>( p_extra )... ); }

		/**
		 * @brief Get pyModule.
		 */
		pybind11::module_ & pyModule() noexcept { return _module.pyModule(); }

		/**
		 * @brief Get python module.
		 */
		void getPythonModule( pybind11::module_ ** p_in ) noexcept { _module.getPythonModule( p_in ); }

	  private:
		/**
		 * @brief Module.
		 */
		VTX::PythonBinding::Wrapper::Module & _module;
	};

	/**
	 * @brief Run on the main thread and get a copy of the result.
	 */
	template<typename Func>
	auto synchronizeCopy( const Func & p_function ) -> std::remove_cvref_t<std::invoke_result_t<const Func &>>
	{
		using Result = std::remove_cvref_t<std::invoke_result_t<const Func &>>;
		return THREAD().synchronize( [ &p_function ]() -> Result { return std::invoke( p_function ); } );
	}

	/**
	 * @brief Wait the caller until the predicate is true on the main thread.
	 */
	template<typename Predicate>
	void waitUntil( const Predicate & p_predicate )
	{
		while ( not synchronizeCopy( p_predicate ) ) {}
	}

	namespace Detail
	{
		/**
		 * @brief Synchronized function wrapper for callable objects.
		 */
		template<typename Func, typename Return, typename... Args>
		auto synchronized( const Func p_function, Return ( Func::* )( Args... ) const )
		{
			return [ p_function ]( const Args... p_args ) -> std::remove_cvref_t<Return>
			{ return synchronizeCopy( [ & ]() -> decltype( auto ) { return p_function( p_args... ); } ); };
		}
	} // namespace Detail

	/**
	 * @brief Functions wrappers.
	 */
	template<typename Func>
	auto synchronized( const Func p_function )
	{ return Detail::synchronized( p_function, &Func::operator() ); }

	template<typename Return, typename... Args>
	auto synchronized( Return ( *const p_function )( Args... ) )
	{
		return [ p_function ]( const Args... p_args ) -> std::remove_cvref_t<Return>
		{ return synchronizeCopy( [ & ]() -> decltype( auto ) { return p_function( p_args... ); } ); };
	}

	template<typename Return, typename Class, typename... Args>
	auto synchronized( Return ( Class::* const p_function )( Args... ) const )
	{
		return [ p_function ]( const Class & p_object, const Args... p_args ) -> std::remove_cvref_t<Return>
		{ return synchronizeCopy( [ & ]() -> decltype( auto ) { return ( p_object.*p_function )( p_args... ); } ); };
	}
} // namespace VTX::App::PythonBinding

#endif
