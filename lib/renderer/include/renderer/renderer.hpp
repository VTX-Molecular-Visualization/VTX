#ifndef __VTX_RENDERE_RENDERER__
#define __VTX_RENDERE_RENDERER__

#include "context/opengl_45.hpp"
#include "render_graph.hpp"
#include "scheduler/depth_first_search.hpp"
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Renderer
	{
	  public:
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;

		Renderer()
		{
			// Passes.
			_renderGraph.addPass(
				"Shading",
				{ Pass::Inputs { { E_INPUT_CHANNEL::COLOR_0, { "G" } }, { E_INPUT_CHANNEL::DEPTH, { "D" } } } } );
			_renderGraph.addPass( "Geometric", {} );
			_renderGraph.addPass( "FXAA", { Pass::Inputs { { E_INPUT_CHANNEL::COLOR_0, { "S" } } } } );
			_renderGraph.addPass( "Depth", { Pass::Inputs { { E_INPUT_CHANNEL::COLOR_0, { "G" } } } } );

			// Links.
			_renderGraph.addLink( "Geometric", "Depth", E_INPUT_CHANNEL::COLOR_0 );
			_renderGraph.addLink( "Geometric", "Shading", E_INPUT_CHANNEL::COLOR_0 );
			_renderGraph.addLink( "Depth", "Shading", E_INPUT_CHANNEL::DEPTH );
			_renderGraph.addLink( "Shading", "FXAA", E_INPUT_CHANNEL::COLOR_0 );

			// Setup.
			_renderGraph.setup();

			_renderGraph.addLink( "FXAA", "Depth", E_INPUT_CHANNEL::COLOR_0 );
			_renderGraph.setup();
		}

		// Debug purposes only.
		inline RenderGraphOpenGL45 & getRenderGraph() { return _renderGraph; }

	  private:
		RenderGraphOpenGL45 _renderGraph;
	};
} // namespace VTX::Renderer

#endif