#include "visualization.hpp"
#include "controller/fps.hpp"
#include "controller/shortcut.hpp"
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
			addItem( new Controller::FPS( VTXApp::get().getScene().getCamera() ) );
			addItem( new Controller::Shortcut() );
		}

		void Visualization::exit() { clear(); }

		void Visualization::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			VTXApp::get().getScene().update( p_deltaTime );

			if ( Setting::Rendering::isActive ) { VTXApp::get().renderScene(); }
		}

	} // namespace State
} // namespace VTX
