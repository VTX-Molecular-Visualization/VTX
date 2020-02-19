#include "state_machine.hpp"
#include "define.hpp"
#include "exception.hpp"
#include "export.hpp"
#include "generic/factory.hpp"
#include "load.hpp"
#include "play.hpp"
#include "visualization.hpp"

namespace VTX
{
	namespace State
	{
		void StateMachine::goToState( const std::string & p_name, void * const p_arg )
		{
			VTX_DEBUG( "Go to state: " + p_name );
			if ( _getItems().find( p_name ) != _getItems().end() ) { _switchState( _getItems().at( p_name ), p_arg ); }
			else
			{
				throw Exception::VTXException( "State not found: " + p_name );
			}
		}

		void StateMachine::_addItems()
		{
			addItem( Generic::create<Load>() );
			addItem( Generic::create<Visualization>() );
			addItem( Generic::create<Play>() );
			addItem( Generic::create<Export>() );
		}

		void StateMachine::_switchState( BaseState * const p_state, void * const p_arg )
		{
			if ( _currentState != nullptr ) { _currentState->exit(); }
			_currentState = p_state;
			if ( _currentState != nullptr ) { _currentState->enter( p_arg ); }
		}

		void StateMachine::update( const double p_deltaTime )
		{
			if ( _currentState != nullptr ) { _currentState->update( p_deltaTime ); }
		}
	}; // namespace State
};	   // namespace VTX
