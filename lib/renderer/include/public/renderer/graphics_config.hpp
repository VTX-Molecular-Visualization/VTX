#ifndef __VTX_RENDERER_GRAPHICS_CONFIG__
#define __VTX_RENDERER_GRAPHICS_CONFIG__

#include <util/constants.hpp>

namespace VTX::Renderer
{

	enum E_GRAPHICS_CONFIG_VALUES // Not a class to avoid static_cast<int>.
	{
		ACTIVE_SSAO,
		SSAO_INTENSITY,
		BLUR_SIZE,

		SHADING_MODE,
		COLOR_LIGHT,
		COLOR_BACKGROUND,
		SPECULAR_FACTOR,
		SHININESS,
		TOON_STEPS,

		ACTIVE_FOG,
		COLOR_FOG,
		FOG_NEAR,
		FOG_FAR,
		FOG_DENSITY,

		ACTIVE_OUTLINE,
		COLOR_OUTLINE,
		OUTLINE_SENSITIVITY,
		OUTLINE_THICKNESS,

		ACTIVE_SELECTION,
		COLOR_SELECTION
	};

	enum struct E_SHADING
	{
		DIFFUSE,
		GLOSSY,
		TOON,
		FLAT,
		COUNT
	};
	constexpr std::string_view SHADING_STR[ int( E_SHADING::COUNT ) ] = { "Diffuse", "Glossy", "Toon", "Flat" };

	// Default values.
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

	constexpr bool	ACTIVE_SSAO_DEFAULT	   = true;
	constexpr float SSAO_INTENSITY_DEFAULT = 5.f;
	constexpr float SSAO_INTENSITY_MIN	   = 1.f;
	constexpr float SSAO_INTENSITY_MAX	   = 20.f;
	constexpr float BLUR_SIZE_DEFAULT	   = 17.f;
	constexpr float BLUR_SIZE_MIN		   = 1.f;
	constexpr float BLUR_SIZE_MAX		   = 99.f;

	constexpr bool			ACTIVE_OUTLINE_DEFAULT		= false;
	const Util::Color::Rgba COLOR_OUTLINE_DEFAULT		= COLOR_WHITE;
	constexpr float			OUTLINE_SENSITIVITY_DEFAULT = 0.5f;
	constexpr float			OUTLINE_SENSITIVITY_MIN		= 0.f;
	constexpr float			OUTLINE_SENSITIVITY_MAX		= 1.f;
	constexpr uint			OUTLINE_THICKNESS_DEFAULT	= 1;
	constexpr uint			OUTLINE_THICKNESS_MIN		= 1;
	constexpr uint			OUTLINE_THICKNESS_MAX		= 5;

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

	constexpr bool			ACTIVE_SELECTION_DEFAULT = true;
	const Util::Color::Rgba COLOR_SELECTION_DEFAULT	 = Util::Color::Rgba( 45, 243, 26 );

	struct GraphicsConfig
	{
		// Shading.
		E_SHADING		  shadingMode;
		Util::Color::Rgba colorLight;
		Util::Color::Rgba colorBackground;
		float			  specularFactor;
		float			  shininess;
		uint			  toonSteps;

		// SSAO.
		bool  activeSSAO;
		float ssaoIntensity;
		float blurSize;

		// Outline.
		bool			  activeOutline;
		Util::Color::Rgba colorOutline;
		float			  outlineSensitivity;
		uint			  outlineThickness;

		// Fog.
		bool			  activeFog;
		Util::Color::Rgba colorFog;
		float			  fogNear;
		float			  fogFar;
		float			  fogDensity;

		// Selection.
		bool			  activeSelection;
		Util::Color::Rgba colorSelection;
	};

	namespace GraphicsConfigs
	{
		inline const GraphicsConfig DEFAULT { // Shading.
											  SHADING_MODE_DEFAULT,
											  COLOR_LIGHT_DEFAULT,
											  COLOR_BACKGROUND_DEFAULT,
											  SPECULAR_FACTOR_DEFAULT,
											  SHININESS_DEFAULT,
											  TOON_STEPS_DEFAULT,
											  // SSAO.
											  ACTIVE_SSAO_DEFAULT,
											  SSAO_INTENSITY_DEFAULT,
											  BLUR_SIZE_DEFAULT,
											  // Outline.
											  ACTIVE_OUTLINE_DEFAULT,
											  COLOR_OUTLINE_DEFAULT,
											  OUTLINE_SENSITIVITY_DEFAULT,
											  OUTLINE_THICKNESS_DEFAULT,
											  // Fog.
											  ACTIVE_FOG_DEFAULT,
											  COLOR_FOG_DEFAULT,
											  FOG_NEAR_DEFAULT,
											  FOG_FAR_DEFAULT,
											  FOG_DENSITY_DEFAULT,
											  // Selection.
											  ACTIVE_SELECTION_DEFAULT,
											  COLOR_SELECTION_DEFAULT
		};
	};

} // namespace VTX::Renderer

#endif
