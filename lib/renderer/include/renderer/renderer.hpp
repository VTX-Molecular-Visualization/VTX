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

		Renderer( const size_t p_width, const size_t p_height )
		{
			using namespace Context;

			DescAttachment imageGeometry { E_FORMAT::RGBA32UI };
			DescAttachment imageColor { E_FORMAT::RGBA16F };
			DescAttachment imagePicking { E_FORMAT::RG32UI };
			DescAttachment imageDepth { E_FORMAT::DEPTH_COMPONENT32F };

			// Geometric.
			_renderGraph.addPass(
				"Geometric",
				{ Pass::Inputs {},
				  Pass::Outputs { { E_CHANNEL::COLOR_0, { "Geometry", imageGeometry } },
								  { E_CHANNEL::COLOR_1, { "Color", imageColor } },
								  { E_CHANNEL::COLOR_2, { "Picking", imagePicking } },
								  { E_CHANNEL::DEPTH, { "Depth", imageDepth } } },
				  Pass::Programs { { "Geometric", std::vector<FilePath> { "default.vert", "geometric.frag" } } } } );

			// Depth.
			_renderGraph.addPass(
				"Linearize depth",
				{ Pass::Inputs { { E_CHANNEL::COLOR_0, { "Depth", imageDepth } } },
				  Pass::Outputs { { E_CHANNEL::COLOR_0, { "", DescAttachment { E_FORMAT::R32F } } } },
				  Pass::Programs {
					  { "LinearizeDepth", std::vector<FilePath> { "default.vert", "linearize_depth.frag" } } } } );

			// Shading.
			_renderGraph.addPass(
				"Shading",
				{ Pass::Inputs { { E_CHANNEL::COLOR_0, { "Geometry", imageGeometry } },
								 { E_CHANNEL::COLOR_1, { "Color", imageColor } } },
				  Pass::Outputs { { E_CHANNEL::COLOR_0, { "", imageColor } } },
				  Pass::Programs { { "Shading", std::vector<FilePath> { "default.vert", "shading.frag" } } } } );

			// FXAA.
			_renderGraph.addPass( "FXAA",
								  { Pass::Inputs { { E_CHANNEL::COLOR_0, { "Image", imageColor } } },
									Pass::Outputs { { E_CHANNEL::COLOR_0, { "", imageColor } } },
									Pass::Programs {} } );

			// Links.
			_renderGraph.addLink( "Geometric", "Linearize depth", E_CHANNEL::DEPTH );
			_renderGraph.addLink( "Geometric", "Shading", E_CHANNEL::COLOR_0 );
			_renderGraph.addLink( "Geometric", "Shading", E_CHANNEL::COLOR_1, E_CHANNEL::COLOR_1 );
			_renderGraph.addLink( "Shading", "FXAA", E_CHANNEL::COLOR_0 );

			// Setup.
			_renderGraph.setup();
		}

		void resize( const size_t p_width, const size_t p_height ) {}

		// Debug purposes only.
		inline RenderGraphOpenGL45 & getRenderGraph() { return _renderGraph; }

	  private:
		RenderGraphOpenGL45 _renderGraph;
	};
} // namespace VTX::Renderer

#endif
