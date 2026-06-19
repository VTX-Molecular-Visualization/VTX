#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_SELECTION__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_SELECTION__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <util/constants.hpp>

namespace VTX::Renderer
{
	constexpr bool			ACTIVE_SELECTION_DEFAULT = true;
	const Util::Color::Rgba COLOR_SELECTION_DEFAULT	 = Util::Color::Rgba( 45, 243, 26 );

	struct SelectionConfig
	{
		Util::Color::Rgba color;
	};

	namespace GraphicsConfigs
	{
		inline const SelectionConfig SELECTION_DEFAULT { COLOR_SELECTION_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct Selection
	{
		inline static const Desc::Key PASS = "Selection";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( "Geometry" )
				.in( p_input )
				.in( "Depth" )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "selection.frag" } )
				.uniform( "Color", COLOR_SELECTION_DEFAULT )
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const SelectionConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( p_config.color );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
