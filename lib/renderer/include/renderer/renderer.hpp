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

		Renderer( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc = nullptr )
		{
			using namespace Context;

			_renderGraph = std::make_unique<RenderGraphOpenGL45>();

			DescAttachment imageGeometry { E_FORMAT::RGBA32UI };
			DescAttachment imageColor { E_FORMAT::RGBA16F };
			DescAttachment imagePicking { E_FORMAT::RG32UI };
			DescAttachment imageDepth { E_FORMAT::DEPTH_COMPONENT32F };

			// Geometric.
			_renderGraph->addPass( "Geometric",
								   { Pass::Inputs {},
									 Pass::Outputs { { E_CHANNEL::COLOR_0, { "Geometry", imageGeometry } },
													 { E_CHANNEL::COLOR_1, { "Color", imageColor } },
													 { E_CHANNEL::COLOR_2, { "Picking", imagePicking } },
													 { E_CHANNEL::DEPTH, { "Depth", imageDepth } } },
									 Pass::Programs { /*{ "Sphere", {"sphere"} }, {"Cylinder", {"cylinder"}} */ } } );

			// Depth.
			_renderGraph->addPass(
				"Linearize depth",
				{ Pass::Inputs { { E_CHANNEL::COLOR_0, { "Depth", imageDepth } } },
				  Pass::Outputs { { E_CHANNEL::COLOR_0, { "", DescAttachment { E_FORMAT::R32F } } } },
				  Pass::Programs { { "LinearizeDepth", { "default.vert", "linearize_depth.frag" } } } } );

			// Shading.
			_renderGraph->addPass(
				"Shading",
				{ Pass::Inputs { { E_CHANNEL::COLOR_0, { "Geometry", imageGeometry } },
								 { E_CHANNEL::COLOR_1, { "Color", imageColor } },
								 { E_CHANNEL::COLOR_2, { "Blur", DescAttachment { E_FORMAT::R16F } } } },
				  Pass::Outputs { { E_CHANNEL::COLOR_0, { "", imageColor } } },
				  Pass::Programs { { "Shading", { "default.vert", "shading.frag" } } } } );

			// FXAA.
			_renderGraph->addPass( "FXAA",
								   { Pass::Inputs { { E_CHANNEL::COLOR_0, { "Image", imageColor } } },
									 Pass::Outputs { { E_CHANNEL::COLOR_0, { "", imageColor } } },
									 Pass::Programs {} } );

			// Links.
			_renderGraph->addLink( "Geometric", "Linearize depth", E_CHANNEL::DEPTH );
			_renderGraph->addLink( "Geometric", "Shading" );
			_renderGraph->addLink( "Geometric", "Shading", E_CHANNEL::COLOR_1, E_CHANNEL::COLOR_1 );
			_renderGraph->addLink( "Shading", "FXAA" );

			// Setup.
			_renderGraph->setup( p_width, p_height, p_shaderPath, p_proc );
		}

		void resize( const size_t p_width, const size_t p_height ) { _renderGraph->resize( p_width, p_height ); }

		// Debug purposes only.
		inline RenderGraphOpenGL45 & getRenderGraph() { return *_renderGraph; }

	  private:
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
	};
} // namespace VTX::Renderer

#endif
