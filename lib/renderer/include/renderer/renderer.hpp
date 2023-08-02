#ifndef __VTX_RENDERE_RENDERER__
#define __VTX_RENDERE_RENDERER__

#include "context/opengl_45.hpp"
#include "render_graph.hpp"
#include "scheduler/dept_first_search.hpp"
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Renderer
	{
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;

	  public:
		Renderer()
		{
			// Passes.
			_renderGraph.addPass( "Geometric", {} );
			_renderGraph.addPass( "Depth", { PassInputs { { E_INPUT_CHANNEL::COLOR_0, { "Geometric" } } } } );
			_renderGraph.addPass( "Shading",
								  { PassInputs { { E_INPUT_CHANNEL::COLOR_0, { "Geometric" } },
												 { E_INPUT_CHANNEL::DEPTH, { "Depth" } } } } );
			_renderGraph.addPass( "FXAA", { PassInputs { { E_INPUT_CHANNEL::COLOR_0, { "Shading" } } } } );

			// Links.
			_renderGraph.addLink( "Geometric", "Depth", E_INPUT_CHANNEL::COLOR_0 );
			_renderGraph.addLink( "Geometric", "Shading", E_INPUT_CHANNEL::COLOR_0 );
			_renderGraph.addLink( "Depth", "Shading", E_INPUT_CHANNEL::DEPTH );
			_renderGraph.addLink( "Shading", "FXAA", E_INPUT_CHANNEL::COLOR_0 );

			// Setup.
			_renderGraph.setup();
		}

		// Debug purposes only.
		inline RenderGraphOpenGL45 & getRenderGraph() { return _renderGraph; }

	  private:
		RenderGraphOpenGL45 _renderGraph;
	};
} // namespace VTX::Renderer

#endif
