#include "state_machine.hpp"
#include "../defines.hpp"
#include "state_loading.hpp"
#include "state_visualization.hpp"
#include <magic_enum.hpp>

namespace VTX
{
	namespace State
	{
		StateMachine::~StateMachine()
		{
			for ( PairEnumToStateSharedPtr pair : _states )
			{
				pair.second.reset();
			}
			_states.clear();
		}

		void StateMachine::init() { _addStates(); }

		void StateMachine::update()
		{
			if ( _currentState != nullptr ) { _currentState->update(); }
		}

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
				_states.try_emplace( p_state->getName(), p_state );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "A state with this name already exists: "
							 + std::string( magic_enum::enum_name( p_state->getName() ) ) );
			}
		}

		void StateMachine::_switchState( const StateSharedPtr p_state )
		{
			VTX_INFO( "Entering state: " + std::string( magic_enum::enum_name( p_state->getName() ) ) );
			if ( _currentState != nullptr ) { _currentState->exit(); }
			_currentState = p_state;
			if ( _currentState != nullptr ) { _currentState->enter(); }
		}
	}; // namespace State
};	   // namespace VTX
