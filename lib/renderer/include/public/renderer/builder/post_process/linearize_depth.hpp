#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_LINEARIZE_DEPTH__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_LINEARIZE_DEPTH__

#include "renderer/graph_builder.hpp"
#include <string_view>

namespace VTX::Renderer::Builder::PostProcess
{
	struct LinearizeDepth
	{
		static constexpr std::string_view PASS	 = "LinearizeDepth";
		static constexpr std::string_view OUTPUT = "Depth";

		static Desc::Key build( GraphBuilder & p_graph )
		{
			p_graph.pass( Desc::Key { PASS } )
				.in( "DepthRaw" )
				.out( Desc::Key { OUTPUT } )
				.program( Desc::Key { PASS } )
				.shaders( { "default.vert", "linearize_depth.frag" } )
				.endProgram()
				.endPass();

			return Desc::Key { OUTPUT };
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
