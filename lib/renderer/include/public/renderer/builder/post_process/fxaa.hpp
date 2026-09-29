#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_FXAA__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_FXAA__

#include "renderer/descriptors.hpp"
#include "renderer/graph_builder.hpp"
#include <string_view>

namespace VTX::Renderer::Builder::PostProcess
{
	struct FXAA
	{
		static constexpr std::string_view PASS = "FXAA";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( Desc::Key { PASS } )
				.settings( { Desc::E_SETTING::SRGB } )
				.in( p_input )
				.out( Desc::Key { PASS } )
				.program( Desc::Key { PASS } )
				.shaders( { "default.vert", "fxaa.frag" } )
				.endProgram()
				.endPass();

			return Desc::Key { PASS };
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
