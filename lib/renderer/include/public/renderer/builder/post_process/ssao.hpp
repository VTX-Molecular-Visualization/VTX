#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_SSAO__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_SSAO__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <utility>

namespace VTX::Renderer
{
	constexpr bool	ACTIVE_SSAO_DEFAULT	   = true;
	constexpr float SSAO_INTENSITY_DEFAULT = 5.f;
	constexpr float SSAO_INTENSITY_MIN	   = 1.f;
	constexpr float SSAO_INTENSITY_MAX	   = 20.f;
	constexpr float BLUR_SIZE_DEFAULT	   = 17.f;
	constexpr float BLUR_SIZE_MIN		   = 1.f;
	constexpr float BLUR_SIZE_MAX		   = 99.f;

	struct SSAOConfig
	{
		float intensity;
		float blurSize;
	};

	namespace GraphicsConfigs
	{
		inline const SSAOConfig SSAO_DEFAULT { SSAO_INTENSITY_DEFAULT, BLUR_SIZE_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct SSAO
	{
		inline static const Desc::Key PASS = "SSAO";

		static Desc::Key build( GraphBuilder & p_graph )
		{
			p_graph.pass( PASS )
				.in( "Geometry" )
				.in( "Noise", "NearestRepeat" )
				.in( "Depth" )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "ssao.frag" } )
				.uniform( "Intensity", SSAO_INTENSITY_DEFAULT, std::pair { SSAO_INTENSITY_MIN, SSAO_INTENSITY_MAX } )
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const SSAOConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( p_config.intensity );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
