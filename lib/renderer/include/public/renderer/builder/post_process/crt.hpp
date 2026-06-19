#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_CRT__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_CRT__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <utility>

namespace VTX::Renderer
{
	constexpr bool	ACTIVE_CRT_DEFAULT			   = false;
	constexpr float CRT_CURVATURE_X_DEFAULT		   = 1.8f;
	constexpr float CRT_CURVATURE_Y_DEFAULT		   = 2.1f;
	constexpr float CRT_CURVATURE_MIN			   = 1.f;
	constexpr float CRT_CURVATURE_MAX			   = 10.f;
	constexpr float CRT_RATIO_DEFAULT			   = 0.75f;
	constexpr float CRT_RATIO_MIN				   = 0.1f;
	constexpr float CRT_RATIO_MAX				   = 1.f;
	constexpr float CRT_GRANINESS_X_DEFAULT		   = 1.35f;
	constexpr float CRT_GRANINESS_Y_DEFAULT		   = 0.35f;
	constexpr float CRT_GRANINESS_MIN			   = 0.f;
	constexpr float CRT_GRANINESS_MAX			   = 5.f;
	constexpr float CRT_VIGNETTE_ROUNDNESS_DEFAULT = 180.f;
	constexpr float CRT_VIGNETTE_ROUNDNESS_MIN	   = 1.f;
	constexpr float CRT_VIGNETTE_ROUNDNESS_MAX	   = 1000.f;
	constexpr float CRT_VIGNETTE_INTENSITY_DEFAULT = 0.85f;
	constexpr float CRT_VIGNETTE_INTENSITY_MIN	   = 0.f;
	constexpr float CRT_VIGNETTE_INTENSITY_MAX	   = 5.f;
	constexpr float CRT_BRIGHTNESS_DEFAULT		   = 2.f;
	constexpr float CRT_BRIGHTNESS_MIN			   = 1.f;
	constexpr float CRT_BRIGHTNESS_MAX			   = 10.f;

	struct CRTConfig
	{
		float curvatureX;
		float curvatureY;
		float ratio;
		float graninessX;
		float graninessY;
		float vignetteRoundness;
		float vignetteIntensity;
		float brightness;
	};

	namespace GraphicsConfigs
	{
		inline const CRTConfig CRT_DEFAULT {
			CRT_CURVATURE_X_DEFAULT,		CRT_CURVATURE_Y_DEFAULT, CRT_RATIO_DEFAULT,
			CRT_GRANINESS_X_DEFAULT,		CRT_GRANINESS_Y_DEFAULT, CRT_VIGNETTE_ROUNDNESS_DEFAULT,
			CRT_VIGNETTE_INTENSITY_DEFAULT, CRT_BRIGHTNESS_DEFAULT
		};
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct CRT
	{
		inline static const Desc::Key PASS = "CRT";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( p_input )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "crt.frag" } )
				.uniform( "Curvature", Vec2f( CRT_CURVATURE_X_DEFAULT, CRT_CURVATURE_Y_DEFAULT ) )
				.uniform( "Ratio", CRT_RATIO_DEFAULT, std::pair { CRT_RATIO_MIN, CRT_RATIO_MAX } )
				.uniform( "GraninessX", CRT_GRANINESS_X_DEFAULT, std::pair { CRT_GRANINESS_MIN, CRT_GRANINESS_MAX } )
				.uniform( "GraninessY", CRT_GRANINESS_Y_DEFAULT, std::pair { CRT_GRANINESS_MIN, CRT_GRANINESS_MAX } )
				.uniform(
					"VignetteRoundness",
					CRT_VIGNETTE_ROUNDNESS_DEFAULT,
					std::pair { CRT_VIGNETTE_ROUNDNESS_MIN, CRT_VIGNETTE_ROUNDNESS_MAX }
				)
				.uniform(
					"VignetteIntensity",
					CRT_VIGNETTE_INTENSITY_DEFAULT,
					std::pair { CRT_VIGNETTE_INTENSITY_MIN, CRT_VIGNETTE_INTENSITY_MAX }
				)
				.uniform( "Brightness", CRT_BRIGHTNESS_DEFAULT, std::pair { CRT_BRIGHTNESS_MIN, CRT_BRIGHTNESS_MAX } )
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const CRTConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( Vec2f( p_config.curvatureX, p_config.curvatureY ) );
			buffer.write( p_config.ratio );
			buffer.write( p_config.graninessX );
			buffer.write( p_config.graninessY );
			buffer.write( p_config.vignetteRoundness );
			buffer.write( p_config.vignetteIntensity );
			buffer.write( p_config.brightness );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
