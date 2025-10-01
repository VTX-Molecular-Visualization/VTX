#ifndef __VTX_APP_ACTION_ACTION_SYSTEM__
#define __VTX_APP_ACTION_ACTION_SYSTEM__

#include "app/action/base_action.hpp"
#include <concepts>
#include <memory>

namespace VTX::App::Action
{
	/**
	 * @brief Concept to check if a type is derived from BaseAction.
	 */
	template<typename T>
	concept ActionConcept = std::derived_from<T, BaseAction>;

	/**
	 * @brief Handle the execution of all actions.
	 */
	class ActionManager
	{
	  public:
		/**
		 * @brief Execute an action of type A with the given arguments.
		 */
		template<ActionConcept A, typename... Args>
		void execute( const Args &... p_args ) const
		{
			std::unique_ptr<BaseAction> a = std::make_unique<A>( p_args... );
			execute( a.get() );
		}

		/**
		 * @brief Execute the given action.
		 */
		void execute( BaseAction * const ) const;
	};
} // namespace VTX::App::Action

#endif
