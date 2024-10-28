#ifndef __VTX_RENDERER__SETTINGS__
#define __VTX_RENDERER__SETTINGS__

#include <util/constants.hpp>

namespace VTX::Renderer
{

	enum struct E_RENDER_SETTINGS
	{
		SSAO_INTENSITY,
		BLUR_SIZE,
		COLOR_BACKGROUND,
		COLOR_LIGHT,
		COLOR_FOG,
		SHADING_MODE,
		SPECULAR_FACTOR,
		SHININESS,
		TOON_STEPS,
		FOG_NEAR,
		FOG_FAR,
		FOG_DENSITY,
		COLOR_OUTLINE,
		OUTLINE_SENSITIVITY,
		OUTLINE_THICKNESS,
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

	const std::string_view SHADING_STR[ int( E_SHADING::COUNT ) ] = { "Diffuse", "Glossy", "Toon", "Flat" };

	constexpr float SSAO_INTENSITY_DEFAULT = 5.f;
	constexpr float SSAO_INTENSITY_MIN	   = 1.f;
	constexpr float SSAO_INTENSITY_MAX	   = 20.f;

	constexpr float BLUR_SIZE_DEFAULT = 17.f;
	constexpr float BLUR_SIZE_MIN	  = 1.f;
	constexpr float BLUR_SIZE_MAX	  = 99.f;

	const Util::Color::Rgba COLOR_BACKGROUND_DEFAULT = COLOR_BLACK;
	const Util::Color::Rgba COLOR_LIGHT_DEFAULT		 = COLOR_WHITE;
	const Util::Color::Rgba COLOR_FOG_DEFAULT		 = COLOR_BLACK;
	constexpr E_SHADING		SHADING_MODE_DEFAULT	 = E_SHADING::DIFFUSE;

	constexpr float SPECULAR_FACTOR_DEFAULT = 0.4f;
	constexpr float SPECULAR_FACTOR_MIN		= 0.f;
	constexpr float SPECULAR_FACTOR_MAX		= 1.f;

	constexpr float SHININESS_DEFAULT = 32.f;
	constexpr float SHININESS_MIN	  = 0.f;
	constexpr float SHININESS_MAX	  = 128.f;

	constexpr uint TOON_STEPS_DEFAULT = 4;
	constexpr uint TOON_STEPS_MIN	  = 1;
	constexpr uint TOON_STEPS_MAX	  = 15;

	constexpr float FOG_NEAR_DEFAULT = 30.f;
	constexpr float FOG_NEAR_MIN	 = 0.f;
	constexpr float FOG_NEAR_MAX	 = 1000.f;

	constexpr float FOG_FAR_DEFAULT = 80.f;
	constexpr float FOG_FAR_MIN		= 0.f;
	constexpr float FOG_FAR_MAX		= 1000.f;

	constexpr float FOG_DENSITY_DEFAULT = 0.f;
	constexpr float FOG_DENSITY_MIN		= 0.f;
	constexpr float FOG_DENSITY_MAX		= 1.f;

	const Util::Color::Rgba COLOR_OUTLINE_DEFAULT		= COLOR_RED;
	constexpr float			OUTLINE_SENSITIVITY_DEFAULT = 0.f;
	constexpr float			OUTLINE_SENSITIVITY_MIN		= 0.f;
	constexpr float			OUTLINE_SENSITIVITY_MAX		= 1.f;
	constexpr uint			OUTLINE_THICKNESS_DEFAULT	= 1;
	constexpr uint			OUTLINE_THICKNESS_MIN		= 1;
	constexpr uint			OUTLINE_THICKNESS_MAX		= 5;

	const Util::Color::Rgba COLOR_SELECTION_DEFAULT = Util::Color::Rgba( 45, 243, 26 );

} // namespace VTX::Renderer

#endif
