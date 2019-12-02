#include "state_machine.hpp"
#include "../defines.hpp"

namespace VTX
{
	namespace State
	{
		StateMachine::~StateMachine()
		{
			for ( auto & [ name, state ] : _states )
			{
				state.reset();
			}
			_states.clear();
		}

		void StateMachine::init() { _addStates(); }

		void StateMachine::goToState( const STATE_NAME p_name )
		{
			try
			{
				_switchState( _states.at( p_name ) );
			}
			catch ( const std::exception )
			{
				VTX_ERROR( "State not found: " + std::string( magic_enum::enum_name( p_name ) ) );
			}
		}

		void StateMachine::_addState( const std::shared_ptr<BaseState> p_state )
		{
			// p_state->init();
			try
			{
				_states.try_emplace( p_state->getName(), p_state );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "A state with this name already exists: "
							 + std::string( magic_enum::enum_name( p_state->getName() ) ) );
			}
		}

		void StateMachine::_switchState( const std::shared_ptr<BaseState> p_state )
		{
			if ( _currentState != nullptr ) { _currentState->exit(); }
			_currentState = p_state;
			if ( _currentState != nullptr ) { _currentState->enter(); }
		}
	}; // namespace State
};	   // namespace VTX
