#include "state_machine.hpp"
#include "define.hpp"
#include "event/event.hpp"
#include "exception.hpp"
#include "export.hpp"
#include "generic/factory.hpp"
#include "id.hpp"
#include "play.hpp"
#include "visualization.hpp"
#include "vtx_app.hpp"
#include "event/event_manager.hpp"

namespace VTX
{
	namespace State
	{
		StateMachine::StateMachine()
		{
			addItem( ID::State::VISUALIZATION, new Visualization() );
			addItem( ID::State::PLAY, new Play() );
			addItem( ID::State::EXPORT, new Export() );
		}

		void StateMachine::goToState( const ID::VTX_ID & p_name, void * const p_arg )
		{
			VTX_DEBUG( "Go to state: " + p_name );
			if ( hasItem( p_name ) )
			{
				VTX_EVENT( new Event::VTXEventValue<ID::VTX_ID>( Event::Global::CHANGE_STATE, p_name ) );
				_switchState( getItem( p_name ), p_arg );
			}
			else
			{
				throw Exception::VTXException( "State not found: " + p_name );
			}
		}

		void StateMachine::_switchState( BaseState * const p_state, void * const p_arg )
		{
			if ( _currentState != nullptr )
			{
				_currentState->exit();
			}
			_currentState = p_state;
			if ( _currentState != nullptr )
			{
				_currentState->enter( p_arg );
			}
		}

		void StateMachine::update( const double & p_deltaTime )
		{
			if ( _currentState != nullptr )
			{
				_currentState->update( p_deltaTime );
			}
		}
	}; // namespace State
};	   // namespace VTX
