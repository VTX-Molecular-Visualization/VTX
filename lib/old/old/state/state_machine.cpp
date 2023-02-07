#include "state_machine.hpp"
#include <util/types.hpp>
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "exception.hpp"
#include "export.hpp"
#include "play.hpp"
#include "visualization.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		StateMachine::StateMachine()
		{
			_states.emplace( ID::State::VISUALIZATION, new Visualization() );
			_states.emplace( ID::State::PLAY, new Play() );
			_states.emplace( ID::State::EXPORT, new Export() );
		}

		StateMachine::~StateMachine()
		{
			for ( const std::pair<const ID::VTX_ID, BaseState * const> & pair : _states )
			{
				delete pair.second;
			}
			_states.clear();
		}

		void StateMachine::goToState( const ID::VTX_ID & p_name, void * const p_arg )
		{
			VTX_DEBUG( "Go to state: " + p_name );
			if ( _states.find( p_name ) != _states.end() )
			{
				VTX_EVENT( new Event::VTXEventValue<ID::VTX_ID>( Event::Global::CHANGE_STATE, p_name ) );
				_switchState( _states[ p_name ], p_arg );
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

		void StateMachine::update( const float & p_deltaTime )
		{
			if ( _currentState != nullptr )
			{
				_currentState->update( p_deltaTime );
			}
		}
	}; // namespace State
};	   // namespace VTX
