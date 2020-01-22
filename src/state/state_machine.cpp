#include "state_machine.hpp"
#include "../defines.hpp"
#include "state_loading.hpp"
#include "state_visualization.hpp"

namespace VTX
{
	namespace State
	{
		StateMachine::~StateMachine() { _states.clear(); }

		void StateMachine::init() { _addStates(); }

		void StateMachine::goToState( const STATE_NAME p_name, void * const p_arg )
		{
			try
			{
				_switchState( _states.at( p_name ), p_arg );
			}
			catch ( const std::exception )
			{
				VTX_ERROR( "State not found: " + ENUM_TO_STRING( p_name ) );
			}
		}

		void StateMachine::_addStates()
		{
			_addState( std::make_shared<StateLoading>( StateLoading() ) );
			_addState( std::make_shared<StateVisualization>( StateVisualization() ) );
		}

		void StateMachine::_addState( const StateSharedPtr p_state )
		{
			// std::static_pointer_cast<StateMachine>( p_state )->init();
			try
			{
				_states.try_emplace( p_state->getStateName(), p_state );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "A state with this name already exists: " + ENUM_TO_STRING( p_state->getStateName() ) );
			}
		}

		void StateMachine::_switchState( const StateSharedPtr p_state, void * const p_arg )
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
