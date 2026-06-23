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
		ACES_FILM,
		COUNT
	};

	constexpr E_TONE_MAPPING TONE_MAPPING_MODE_DEFAULT	   = E_TONE_MAPPING::REINHARD;
	constexpr float			 TONE_MAPPING_EXPOSURE_DEFAULT = 2.f;
	constexpr float			 TONE_MAPPING_EXPOSURE_MIN	   = 0.f;
	constexpr float			 TONE_MAPPING_EXPOSURE_MAX	   = 10.f;

	struct ToneMappingConfig
	{
		E_TONE_MAPPING mode;
		float		   exposure;
	};

	namespace GraphicsConfigs
	{
		inline const ToneMappingConfig TONE_MAPPING_DEFAULT { TONE_MAPPING_MODE_DEFAULT,
															  TONE_MAPPING_EXPOSURE_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

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
				.uniform(
					"Mode",
					uint32_t( TONE_MAPPING_MODE_DEFAULT ),
					std::pair { 0u, uint32_t( E_TONE_MAPPING::COUNT ) - 1u }
				)
				.uniform(
					"Exposure",
					TONE_MAPPING_EXPOSURE_DEFAULT,
					std::pair { TONE_MAPPING_EXPOSURE_MIN, TONE_MAPPING_EXPOSURE_MAX }
				)
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const ToneMappingConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( uint32_t( p_config.mode ) );
			buffer.write( p_config.exposure );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
