#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_COLORIZE__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_COLORIZE__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <util/constants.hpp>

namespace VTX::Renderer
{
	constexpr bool			ACTIVE_COLORIZE_DEFAULT = false;
	const Util::Color::Rgba COLORIZE_COLOR_DEFAULT	= COLOR_WHITE;

	struct ColorizeConfig
	{
		Util::Color::Rgba color;
	};

	namespace GraphicsConfigs
	{
		inline const ColorizeConfig COLORIZE_DEFAULT { COLORIZE_COLOR_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct Colorize
	{
		inline static const Desc::Key PASS = "Colorize";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( p_input )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "colorize.frag" } )
				.uniform( "Color", COLORIZE_COLOR_DEFAULT.toLinear() )
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const ColorizeConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( p_config.color.toLinear() );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
