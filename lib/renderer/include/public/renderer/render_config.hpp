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

	namespace Default
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
