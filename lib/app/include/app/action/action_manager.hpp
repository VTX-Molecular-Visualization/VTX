#ifndef __VTX_APP_ACTION_ACTION_MANAGER__
#define __VTX_APP_ACTION_ACTION_MANAGER__

#include <concepts>
#include <memory>
#include <util/chrono.hpp>
#include <util/hashing.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::App::Action
{

	/**
	 * @brief Handle the execution of all actions.
	 */
	class ActionManager
	{
	  public:
		/**
		 * @brief Execute an action of type A with the given arguments.
		 */
		template<typename A, typename... Args>
		void execute( Args &&... p_args ) const
		{
			A a;
			execute( a, std::forward<Args>( p_args )... );
		}

		/**
		 * @brief Execute the given action.
		 */
		template<typename A, typename... Args>
		void execute( A & p_action, Args &&... p_args ) const
		{
			const auto actionName = Util::typeName<A>();
			VTX_DEBUG( "ActionSystem::execute( {} )", actionName );
			try
			{
				auto duration = Util::CHRONO_CPU( [ & ]() { p_action.execute( std::forward<Args>( p_args )... ); } );
				VTX_DEBUG(
					"ActionSystem::execute( {} ) - done ({})", actionName, Util::String::durationToStr( duration )
				);
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Error with action of type {} : {}", actionName, p_e.what() );
				return;
			}
		}
	};
} // namespace VTX::App::Action

#endif
