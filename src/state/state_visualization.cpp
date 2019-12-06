#include "state_visualization.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void StateVisualization::enter()
		{
			// Start the renderer.
			VTXApp::get().initRenderer();
		}

		void StateVisualization::exit() {}
	} // namespace State
} // namespace VTX
