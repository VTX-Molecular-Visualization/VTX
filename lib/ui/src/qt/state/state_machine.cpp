#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/export.hpp"
#include "ui/qt/state/play.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/core/event/vtx_event.hpp>
#include <app/event.hpp>
#include <app/event/global.hpp>
#include <app/vtx_app.hpp>
#include <util/exceptions.hpp>

namespace VTX::UI::QT::State
{
	StateMachine::StateMachine()
	{
		_states.emplace( ID::State::VISUALIZATION, new Visualization() );
		_states.emplace( ID::State::PLAY, new Play() );
		_states.emplace( ID::State::EXPORT, new Export() );
	}

	StateMachine::~StateMachine()
	{
		for ( const std::pair<const App::VTX_ID, BaseState * const> & pair : _states )
		{
			delete pair.second;
		}
		_states.clear();
	}

	void StateMachine::goToState( const App::VTX_ID & p_name, void * const p_arg )
	{
		VTX_DEBUG( "Go to state: " + p_name );
		if ( _states.find( p_name ) != _states.end() )
		{
			VTX_EVENT<const App::VTX_ID &>( VTX::App::Event::Global::CHANGE_STATE, p_name );
			_switchState( _states[ p_name ], p_arg );
		}
		else
		{
			throw VTXException( "State not found: " + p_name );
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
}; // namespace VTX::UI::QT::State
