#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_BLUR__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_BLUR__

#include "renderer/binary_buffer.hpp"
#include "renderer/builder/post_process/ssao.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <utility>

namespace VTX::Renderer::Builder::PostProcess
{
	struct BlurX
	{
		inline static const Desc::Key PASS = "BlurX";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( p_input )
				.in( "Depth" )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "blur.frag" } )
				.uniform( "Direction", Vec2i( 1, 0 ) )
				.uniform( "Size", BLUR_SIZE_DEFAULT, std::pair { BLUR_SIZE_MIN, BLUR_SIZE_MAX } )
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const SSAOConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( Vec2i( 1, 0 ) );
			buffer.write( p_config.blurSize );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};

	struct BlurY
	{
		inline static const Desc::Key PASS = "BlurY";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( p_input )
				.in( "Depth" )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "blur.frag" } )
				.uniform( "Direction", Vec2i( 0, 1 ) )
				.uniform( "Size", BLUR_SIZE_DEFAULT, std::pair { BLUR_SIZE_MIN, BLUR_SIZE_MAX } )
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const SSAOConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( Vec2i( 0, 1 ) );
			buffer.write( p_config.blurSize );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
