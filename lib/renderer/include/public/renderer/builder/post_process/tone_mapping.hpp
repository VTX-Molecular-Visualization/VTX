#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_TONE_MAPPING__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_TONE_MAPPING__

#include "renderer/graph_builder.hpp"

namespace VTX::Renderer::Builder::PostProcess
{
	struct ToneMapping
	{
		inline static const Desc::Key PASS = "ToneMapping";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( p_input )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "tone_mapping.frag" } )
				.endProgram()
				.endPass();

			return PASS;
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
