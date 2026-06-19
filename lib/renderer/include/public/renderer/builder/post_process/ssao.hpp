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
	constexpr float SSAO_RADIUS_DEFAULT	   = 0.5f;
	constexpr float SSAO_RADIUS_MIN		   = 0.1f;
	constexpr float SSAO_RADIUS_MAX		   = 20.f;
	constexpr float SSAO_SCALE_DEFAULT	   = 1.f;
	constexpr float SSAO_SCALE_MIN		   = 0.25f;
	constexpr float SSAO_SCALE_MAX		   = 4.f;
	constexpr float BLUR_SIZE_DEFAULT	   = 17.f;
	constexpr float BLUR_SIZE_MIN		   = 1.f;
	constexpr float BLUR_SIZE_MAX		   = 99.f;

	enum struct E_SSAO_METHOD
	{
		SSAO,
		SSAO_LINE,
		SAO,
		BMGTAO,
		HBAO,
		COUNT
	};

	constexpr E_SSAO_METHOD SSAO_METHOD_DEFAULT = E_SSAO_METHOD::SSAO;

	struct SSAOConfig
	{
		E_SSAO_METHOD method;
		float		  intensity;
		float		  radius;
		float		  scale;
		float		  blurSize;
	};

	namespace GraphicsConfigs
	{
		inline const SSAOConfig SSAO_DEFAULT { SSAO_METHOD_DEFAULT,
											   SSAO_INTENSITY_DEFAULT,
											   SSAO_RADIUS_DEFAULT,
											   SSAO_SCALE_DEFAULT,
											   BLUR_SIZE_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct SSAO
	{
		inline static const Desc::Key PASS = "SSAO";

		static Desc::Key programKey( const E_SSAO_METHOD p_method )
		{
			switch ( p_method )
			{
			case E_SSAO_METHOD::SSAO: return "SSAO.SSAO";
			case E_SSAO_METHOD::SSAO_LINE: return "SSAO.Line";
			case E_SSAO_METHOD::SAO: return "SSAO.SAO";
			case E_SSAO_METHOD::BMGTAO: return "SSAO.BMGTAO";
			case E_SSAO_METHOD::HBAO: return "SSAO.HBAO";
			case E_SSAO_METHOD::COUNT: break;
			}

			return "SSAO.SSAO";
		}

		static Desc::Key build( GraphBuilder & p_graph, const E_SSAO_METHOD p_method )
		{
			FilePath  shader	  = "ssao.frag";
			Desc::Key programName = programKey( p_method );
			switch ( p_method )
			{
			case E_SSAO_METHOD::SSAO: shader = "ssao.frag"; break;
			case E_SSAO_METHOD::SSAO_LINE: shader = "ssao_line.frag"; break;
			case E_SSAO_METHOD::SAO: shader = "sao.frag"; break;
			case E_SSAO_METHOD::BMGTAO: shader = "bmgtao.frag"; break;
			case E_SSAO_METHOD::HBAO: shader = "hbao.frag"; break;
			case E_SSAO_METHOD::COUNT: break;
			}

			auto pass = p_graph.pass( PASS ).in( "Geometry" ).in( "Noise", "NearestRepeat" ).in( "Depth" );
			if ( p_method == E_SSAO_METHOD::BMGTAO )
			{
				pass.in( "Color" );
			}

			auto program
				= pass.out( PASS )
					  .program( programName )
					  .shaders( { "default.vert", shader } )
					  .uniform(
						  "Intensity", SSAO_INTENSITY_DEFAULT, std::pair { SSAO_INTENSITY_MIN, SSAO_INTENSITY_MAX }
					  )
					  .uniform( "Radius", SSAO_RADIUS_DEFAULT, std::pair { SSAO_RADIUS_MIN, SSAO_RADIUS_MAX } )
					  .uniform( "Scale", SSAO_SCALE_DEFAULT, std::pair { SSAO_SCALE_MIN, SSAO_SCALE_MAX } );

			program.endProgram().endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const SSAOConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( p_config.intensity );
			buffer.write( p_config.radius );
			buffer.write( p_config.scale );
			buffer.close();

			p_context.setBuffer( { programKey( p_config.method ) }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
