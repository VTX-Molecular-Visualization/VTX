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

		void StateMachine::_addState( const std::shared_ptr<BaseState> p_state )
		{
			p_state->init();
			try
			{
				_states.try_emplace( p_state->getName(), p_state );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "A view with this type already exists: "
							 + std::string( magic_enum::enum_name( p_state->getName() ) ) );
			}
		}
	}; // namespace State
};	   // namespace VTX
