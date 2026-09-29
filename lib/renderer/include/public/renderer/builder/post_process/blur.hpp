#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_BLUR__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_BLUR__

#include "renderer/binary_buffer.hpp"
#include "renderer/builder/post_process/ssao.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <string_view>
#include <utility>

namespace VTX::Renderer::Builder::PostProcess
{
	struct BlurX
	{
		static constexpr std::string_view PASS = "BlurX";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( Desc::Key { PASS } )
				.in( p_input )
				.in( "Depth" )
				.out( Desc::Key { PASS } )
				.program( Desc::Key { PASS } )
				.shaders( { "default.vert", "blur.frag" } )
				.uniform( "Direction", Vec2i( 1, 0 ) )
				.uniform( "Size", BLUR_SIZE_DEFAULT, std::pair { BLUR_SIZE_MIN, BLUR_SIZE_MAX } )
				.uniform( "Scale", SSAO_SCALE_DEFAULT, std::pair { SSAO_SCALE_MIN, SSAO_SCALE_MAX } )
				.endProgram()
				.endPass();

			return Desc::Key { PASS };
		}

		static void upload( Context::ContextWrapper & p_context, const SSAOConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( Vec2i( 1, 0 ) );
			buffer.write( p_config.blurSize );
			buffer.write( p_config.scale );
			buffer.close();

			p_context.setBuffer( { Desc::Key { PASS } }, buffer.bytes() );
		}
	};

	struct BlurY
	{
		static constexpr std::string_view PASS = "BlurY";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( Desc::Key { PASS } )
				.in( p_input )
				.in( "Depth" )
				.out( Desc::Key { PASS } )
				.program( Desc::Key { PASS } )
				.shaders( { "default.vert", "blur.frag" } )
				.uniform( "Direction", Vec2i( 0, 1 ) )
				.uniform( "Size", BLUR_SIZE_DEFAULT, std::pair { BLUR_SIZE_MIN, BLUR_SIZE_MAX } )
				.uniform( "Scale", SSAO_SCALE_DEFAULT, std::pair { SSAO_SCALE_MIN, SSAO_SCALE_MAX } )
				.endProgram()
				.endPass();

			return Desc::Key { PASS };
		}

		static void upload( Context::ContextWrapper & p_context, const SSAOConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( Vec2i( 0, 1 ) );
			buffer.write( p_config.blurSize );
			buffer.write( p_config.scale );
			buffer.close();

			p_context.setBuffer( { Desc::Key { PASS } }, buffer.bytes() );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
