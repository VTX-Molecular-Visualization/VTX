#include "app/action/action_manager.hpp"
#include <exception>
#include <typeinfo>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::App::Action
{

	void ActionManager::execute( BaseAction * const p_action ) const
	{
		VTX_DEBUG( "ActionSystem::execute( {} )", typeid( *p_action ).name() );
		try
		{
			auto duration = Util::CHRONO_CPU( [ p_action ]() { p_action->execute(); } );
			VTX_DEBUG(
				"ActionSystem::execute( {} ) - done ({})",
				typeid( *p_action ).name(),
				Util::String::durationToStr( duration )
			);
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Error with action of type {} : {}", typeid( *p_action ).name(), p_e.what() );
			return;
		}
	}

} // namespace VTX::App::Action
