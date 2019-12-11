#include "state_visualization.hpp"
#include "../controller/controller_freefly.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void StateVisualization::enter()
		{
			// Init the renderer.
			VTXApp::get().initRenderer();
			// Create controller.
			_controller = new Controller::ControllerFreefly( VTXApp::get().getScene().getCamera() );
		}

		void StateVisualization::exit() { delete _controller; }
	} // namespace State
} // namespace VTX
