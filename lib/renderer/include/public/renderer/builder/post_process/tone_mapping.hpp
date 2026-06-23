#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_TONE_MAPPING__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_TONE_MAPPING__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <cstdint>
#include <utility>

namespace VTX::Renderer
{
	enum struct E_TONE_MAPPING : uint32_t
	{
		NONE,
		REINHARD,
		COUNT
	};
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct ToneMapping
	{
		inline static const Desc::Key		   PASS		= "ToneMapping";
		inline static constexpr E_TONE_MAPPING HDR_MODE = E_TONE_MAPPING::REINHARD;

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( p_input )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "tone_mapping.frag" } )
				.uniform(
					"Mode", uint32_t( E_TONE_MAPPING::NONE ), std::pair { 0u, uint32_t( E_TONE_MAPPING::COUNT ) - 1u }
				)
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const bool p_hdr )
		{
			BinaryBuffer140 buffer;
			buffer.write( uint32_t( p_hdr ? HDR_MODE : E_TONE_MAPPING::NONE ) );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
