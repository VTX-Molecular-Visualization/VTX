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

			_getItem( ID::Controller::TRACKBALL )->setActive( false );
		}

		void Visualization::exit() { Generic::HasCollection<Controller::BaseController>::clean(); }

		void Visualization::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			VTXApp::get().getScene().update( p_deltaTime );

			if ( Setting::Rendering::isActive )
			{
				VTXApp::get().renderScene();
			}
		}

		void Visualization::toggleController()
		{
			_getItem( _controller )->setActive( false );

			if ( _controller == ID::Controller::FREEFLY )
			{
				_controller = ID::Controller::TRACKBALL;
			}
			else
			{
				_controller = ID::Controller::FREEFLY;
			}
			_getItem( _controller )->setActive( true );
		}

	} // namespace State
} // namespace VTX
