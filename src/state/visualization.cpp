#include "visualization.hpp"
#include "controller/freefly.hpp"
#include "controller/shortcut.hpp"
#include "controller/trackball.hpp"
#include "generic/factory.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void Visualization::enter( void * const )
		{
			// Create controller.
			addItem( Generic::create<Controller::Freefly>() );
			addItem( Generic::create<Controller::Trackball>() );
			addItem( Generic::create<Controller::Shortcut>() );

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

		void Visualization::update( const double p_deltaTime )
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
		}

	} // namespace State
} // namespace VTX
