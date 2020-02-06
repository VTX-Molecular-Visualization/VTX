#include "state_visualization.hpp"
#include "controller/controller_fps.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void StateVisualization::enter( void * const )
		{
			// Init the renderer.
			VTXApp::get().initRenderer();
			// Create controller.
			_controller = new Controller::ControllerFPS( VTXApp::get().getScene().getCamera() );
		}

		void StateVisualization::exit() { delete _controller; }

		void StateVisualization::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			VTXApp::get().getScene().update( p_deltaTime );
			VTXApp::get().renderScene();
		}

	} // namespace State
} // namespace VTX
