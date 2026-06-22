#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_FXAA__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_FXAA__

#include "renderer/descriptors.hpp"
#include "renderer/graph_builder.hpp"

namespace VTX::Renderer::Builder::PostProcess
{
	struct FXAA
	{
		inline static const Desc::Key PASS = "FXAA";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.settings( { Desc::E_SETTING::SRGB } )
				.in( p_input )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "fxaa.frag" } )
				.endProgram()
				.endPass();

			return PASS;
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
