#include "state_machine.hpp"
#include "define.hpp"
#include "loading.hpp"
#include "playing.hpp"
#include "visualization.hpp"

namespace VTX
{
	namespace State
	{
		void StateMachine::goToState( const std::string & p_name, void * const p_arg )
		{
			try
			{
				_switchState( _getItems().at( p_name ), p_arg );
			}
			catch ( const std::exception )
			{
				VTX_ERROR( "State not found: " + p_name );
			}
		}

		void StateMachine::_addItems()
		{
			addItem( new Loading() );
			addItem( new Visualization() );
			addItem( new Playing() );
		}

		void StateMachine::_switchState( BaseState * const p_state, void * const p_arg )
		{
			if ( _currentState != nullptr ) { _currentState->exit(); }
			_currentState = p_state;
			if ( _currentState != nullptr ) { _currentState->enter( p_arg ); }
		}

		void StateMachine::handleEvent( const SDL_Event & p_event, void * const p_arg )
		{
			if ( _currentState != nullptr ) { _currentState->handleEvent( p_event, p_arg ); }
		}

		void StateMachine::update( const double p_deltaTime )
		{
			if ( _currentState != nullptr ) { _currentState->update( p_deltaTime ); }
		}
	}; // namespace State
};	   // namespace VTX
