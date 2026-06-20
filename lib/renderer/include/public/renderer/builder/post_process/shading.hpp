#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_SHADING__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_SHADING__

#include "renderer/binary_buffer.hpp"
#include "renderer/builder/post_process/ssao.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <optional>
#include <string_view>
#include <util/constants.hpp>
#include <utility>

namespace VTX::Renderer
{
	enum struct E_SHADING
	{
		DIFFUSE,
		GLOSSY,
		TOON,
		FLAT,
		COUNT
	};

	constexpr E_SHADING		SHADING_MODE_DEFAULT	 = E_SHADING::DIFFUSE;
	const Util::Color::Rgba COLOR_LIGHT_DEFAULT		 = COLOR_WHITE;
	const Util::Color::Rgba COLOR_BACKGROUND_DEFAULT = COLOR_BLACK;
	constexpr float			SPECULAR_FACTOR_DEFAULT	 = 0.4f;
	constexpr float			SPECULAR_FACTOR_MIN		 = 0.f;
	constexpr float			SPECULAR_FACTOR_MAX		 = 1.f;
	constexpr float			SHININESS_DEFAULT		 = 32.f;
	constexpr float			SHININESS_MIN			 = 0.f;
	constexpr float			SHININESS_MAX			 = 128.f;
	constexpr uint			TOON_STEPS_DEFAULT		 = 4;
	constexpr uint			TOON_STEPS_MIN			 = 1;
	constexpr uint			TOON_STEPS_MAX			 = 15;

	constexpr bool			ACTIVE_FOG_DEFAULT	= false;
	const Util::Color::Rgba COLOR_FOG_DEFAULT	= COLOR_GREY;
	constexpr float			FOG_NEAR_DEFAULT	= 30.f;
	constexpr float			FOG_NEAR_MIN		= 0.f;
	constexpr float			FOG_NEAR_MAX		= 1000.f;
	constexpr float			FOG_FAR_DEFAULT		= 80.f;
	constexpr float			FOG_FAR_MIN			= 0.f;
	constexpr float			FOG_FAR_MAX			= 1000.f;
	constexpr float			FOG_DENSITY_DEFAULT = 0.5f;
	constexpr float			FOG_DENSITY_MIN		= 0.f;
	constexpr float			FOG_DENSITY_MAX		= 1.f;

	struct ShadingConfig
	{
		E_SHADING				mode;
		Util::Color::Rgba		colorLight;
		Util::Color::Rgba		colorBackground;
		float					specularFactor;
		float					shininess;
		uint					toonSteps;
		std::optional<FilePath> environmentPath;
		uint32_t				environmentFaceSize = 1024;
		float					environmentExposure = 1.f;
		float					environmentRotation = 0.f;
	};

	struct FogConfig
	{
		Util::Color::Rgba color;
		float			  near;
		float			  far;
		float			  density;
	};

	namespace GraphicsConfigs
	{
		inline const ShadingConfig SHADING_DEFAULT { SHADING_MODE_DEFAULT,	   COLOR_LIGHT_DEFAULT,
													 COLOR_BACKGROUND_DEFAULT, SPECULAR_FACTOR_DEFAULT,
													 SHININESS_DEFAULT,		   TOON_STEPS_DEFAULT };

		inline const FogConfig FOG_DEFAULT { COLOR_FOG_DEFAULT,
											 FOG_NEAR_DEFAULT,
											 FOG_FAR_DEFAULT,
											 FOG_DENSITY_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct Shading
	{
		inline static const Desc::Key PASS				  = "Shading";
		inline static const Desc::Key OUTPUT			  = "Shaded";
		inline static const Desc::Key ENVIRONMENT_TEXTURE = "EnvMap";
		inline static const Desc::Key ENVIRONMENT_SAMPLER = "EnvMapSampler";

		static Desc::Key build( GraphBuilder & p_graph, const bool p_enableSSAO )
		{
			p_graph.pass( PASS )
				.in( "Geometry" )
				.in( "Color" )
				.in( "BlurY", p_enableSSAO ? "NearestClamp" : "NearestRepeat" )
				.in( "Depth" )
				.in( ENVIRONMENT_TEXTURE, ENVIRONMENT_SAMPLER )
				.out( OUTPUT )
				.program( PASS )
				.shaders( { "shading.vert", "shading.frag" } )
				.uniform( "BackgroundColor", COLOR_BACKGROUND_DEFAULT )
				.uniform( "LightColor", COLOR_LIGHT_DEFAULT )
				.uniform( "FogColor", COLOR_FOG_DEFAULT )
				.uniform(
					"Mode",
					static_cast<unsigned int>( SHADING_MODE_DEFAULT ),
					std::pair { static_cast<unsigned int>( E_SHADING::DIFFUSE ),
								static_cast<unsigned int>( E_SHADING::COUNT ) - 1.0 }
				)
				.uniform(
					"SpecularFactor", SPECULAR_FACTOR_DEFAULT, std::pair { SPECULAR_FACTOR_MIN, SPECULAR_FACTOR_MAX }
				)
				.uniform( "Shininess", SHININESS_DEFAULT, std::pair { SHININESS_MIN, SHININESS_MAX } )
				.uniform( "ToonSteps", TOON_STEPS_DEFAULT, std::pair { TOON_STEPS_MIN, TOON_STEPS_MAX } )
				.uniform( "FogNear", FOG_NEAR_DEFAULT, std::pair { FOG_NEAR_MIN, FOG_NEAR_MAX } )
				.uniform( "FogFar", FOG_FAR_DEFAULT, std::pair { FOG_FAR_MIN, FOG_FAR_MAX } )
				.uniform( "FogDensity", FOG_DENSITY_DEFAULT, std::pair { FOG_DENSITY_MIN, FOG_DENSITY_MAX } )
				.uniform( "SSAOScale", SSAO_SCALE_DEFAULT, std::pair { SSAO_SCALE_MIN, SSAO_SCALE_MAX } )
				.uniform( "EnvironmentEnabled", uint32_t( 0 ) )
				.uniform( "EnvironmentExposure", 1.f )
				.uniform( "EnvironmentRotation", 0.f )
				.endProgram()
				.endPass();

			return OUTPUT;
		}

		static void upload(
			Context::ContextWrapper &		  p_context,
			const ShadingConfig &			  p_config,
			const std::optional<FogConfig> &  p_fog,
			const std::optional<SSAOConfig> & p_ssao
		)
		{
			BinaryBuffer140	  buffer;
			const FogConfig & fog = p_fog.value_or( GraphicsConfigs::FOG_DEFAULT );

			buffer.write( p_config.colorBackground );
			buffer.write( p_config.colorLight );
			buffer.write( fog.color );
			buffer.write( uint32_t( p_config.mode ) );
			buffer.write( p_config.specularFactor );
			buffer.write( p_config.shininess );
			buffer.write( p_config.toonSteps );
			buffer.write( fog.near );
			buffer.write( fog.far );
			buffer.write( p_fog ? fog.density : 0.f );
			buffer.write( p_ssao ? p_ssao->scale : SSAO_SCALE_DEFAULT );
			buffer.write( uint32_t( p_config.environmentPath.has_value() ) );
			buffer.write( p_config.environmentExposure );
			buffer.write( p_config.environmentRotation );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}

		static void loadEnvironment( Context::ContextWrapper &, const ShadingConfig & );
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
