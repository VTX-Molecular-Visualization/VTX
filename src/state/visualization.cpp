#include "visualization.hpp"
#include "controller/freefly.hpp"
#include "controller/shortcut.hpp"
#include "controller/trackball.hpp"
#include "event/event.hpp"
#include "generic/factory.hpp"
#include "vtx_app.hpp"
#include "event/event_manager.hpp"

namespace VTX
{
	namespace State
	{
		Visualization::Visualization()
		{
			_registerEvent( Event::Global::MOLECULE_ADDED );
			_registerEvent( Event::Global::MOLECULE_REMOVED );
			_registerEvent( Event::Global::MESH_ADDED );
			_registerEvent( Event::Global::MESH_REMOVED );
		}

		void Visualization::enter( void * const )
		{
			// Create controller.
			addItem( ID::Controller::FREEFLY, new Controller::Freefly() );
			addItem( ID::Controller::TRACKBALL, new Controller::Trackball() );
			addItem( ID::Controller::SHORTCUT, new Controller::Shortcut() );

			if ( _controller == ID::Controller::FREEFLY )
			{
				getItem<Controller::Trackball>( ID::Controller::TRACKBALL )->setActive( false );
			}
			else
			{
				getItem<Controller::Freefly>( ID::Controller::FREEFLY )->setActive( false );
			}
		}

		void Visualization::exit() { Generic::HasCollection<Controller::BaseController>::clear(); }

		void Visualization::update( const double & p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			VTXApp::get().getScene().update( p_deltaTime );

			if ( VTX_SETTING().activeRenderer )
			{
				VTXApp::get().renderScene();
			}
		}

		void Visualization::toggleController()
		{
			getItem( _controller )->setActive( false );

			if ( _controller == ID::Controller::FREEFLY )
			{
				_controller = ID::Controller::TRACKBALL;
			}
			else
			{
				_controller = ID::Controller::FREEFLY;
			}
			getItem( _controller )->setActive( true );

			VTX_EVENT( new Event::VTXEventPtr( Event::Global::CONTROLLER_CHANGE, &_controller ) );
		}
		void Visualization::setController( const ID::VTX_ID & p_controllerId )
		{
			// Do nothing if id not in collection or already in use
			if ( !hasItem( p_controllerId ) || _controller == p_controllerId )
				return;

			getItem( _controller )->setActive( false );
			_controller = p_controllerId;
			getItem( _controller )->setActive( true );

			VTX_EVENT( new Event::VTXEventPtr( Event::Global::CONTROLLER_CHANGE, &_controller ) );
		}

		void Visualization::recenter() { getItem<VTX::Controller::BaseCameraController>( _controller )->reset(); }

		void Visualization::receiveEvent( const Event::VTXEvent & p_event ) { recenter(); }

	} // namespace State
} // namespace VTX
