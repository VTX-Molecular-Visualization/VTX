#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_LINEARIZE_DEPTH__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_LINEARIZE_DEPTH__

#include "renderer/graph_builder.hpp"

namespace VTX::Renderer::Builder::PostProcess
{
	struct LinearizeDepth
	{
		inline static const Desc::Key PASS	 = "LinearizeDepth";
		inline static const Desc::Key OUTPUT = "Depth";

		static Desc::Key build( GraphBuilder & p_graph )
		{
			p_graph.pass( PASS )
				.in( "DepthRaw" )
				.out( OUTPUT )
				.program( PASS )
				.shaders( { "default.vert", "linearize_depth.frag" } )
				.endProgram()
				.endPass();

			return OUTPUT;
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
