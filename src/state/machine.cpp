#include "machine.hpp"
#include "define.hpp"
#include "state_loading.hpp"
#include "state_playing.hpp"
#include "state_visualization.hpp"

namespace VTX
{
	namespace State
	{
		void Machine::goToState( const std::string & p_name, void * const p_arg )
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

		void Machine::_addItems()
		{
			addItem( new StateLoading() );
			addItem( new StateVisualization() );
			addItem( new StatePlaying() );
		}

		void Machine::_switchState( BaseState * const p_state, void * const p_arg )
		{
			if ( _currentState != nullptr ) { _currentState->exit(); }
			_currentState = p_state;
			if ( _currentState != nullptr ) { _currentState->enter( p_arg ); }
		}

		void Machine::handleEvent( const SDL_Event & p_event, const double p_deltaTime, void * const p_arg )
		{
			if ( _currentState != nullptr ) { _currentState->handleEvent( p_event, p_deltaTime, p_arg ); }
		}

		void Machine::update( const double p_deltaTime )
		{
			if ( _currentState != nullptr ) { _currentState->update( p_deltaTime ); }
		}
	}; // namespace State
};	   // namespace VTX
