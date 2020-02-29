#include "visualization.hpp"
#include "controller/Freefly.hpp"
#include "controller/shortcut.hpp"
#include "generic/factory.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void Visualization::enter( void * const )
		{
			// Init the renderer.
			VTXApp::get().initRenderer();
			// Create controller.
			addItem( Generic::create<Controller::Freefly>() );
			addItem( Generic::create<Controller::Shortcut>() );
		}

		void Visualization::exit() { Generic::HasCollection<Controller::BaseController>::clean(); }

		void Visualization::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			VTXApp::get().getScene().update( p_deltaTime );

			if ( Setting::Rendering::isActive ) { VTXApp::get().renderScene(); }
		}

	} // namespace State
} // namespace VTX
