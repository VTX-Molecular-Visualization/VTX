#include "visualization.hpp"
#include "controller/freefly.hpp"
#include "controller/orbit.hpp"
#include "controller/shortcut.hpp"
#include "generic/factory.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void Visualization::enter( void * const )
		{
			// Create controller.
			addItem( Generic::create<Controller::Orbit>() );
			addItem( Generic::create<Controller::Shortcut>() );
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
			if ( _controller == ID::Controller::FREEFLY )
			{
				_controller = ID::Controller::ORBIT;
			}
			else
			{
				_controller = ID::Controller::FREEFLY;
			}
		}

	} // namespace State
} // namespace VTX
