#ifndef __VTX_RENDERER_PROXY_RENDER_SETTINGS__
#define __VTX_RENDERER_PROXY_RENDER_SETTINGS__

#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{
	// TODO: to handle multiples render settings in the same scene.

	/**
	 * @brief Render settings data provider.
	 */

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

	struct RenderSettings
	{
		float			  ssaoIntensity;
		float			  blurSize;
		Util::Color::Rgba colorBackground;
		Util::Color::Rgba colorLight;
		Util::Color::Rgba colorFog;
		uint			  shadingMode;
		float			  specularFactor;
		float			  shininess;
		uint			  toonSteps;
		float			  fogNear;
		float			  fogFar;
		float			  fogDensity;
		Util::Color::Rgba colorOutline;
		float			  outlineSensitivity;
		uint			  outlineThickness;
		Util::Color::Rgba colorSelection;

		Util::Callback<float>					  onSSAOIntensity;
		Util::Callback<float>					  onBlurSize;
		Util::Callback<const Util::Color::Rgba &> onColorBackground;
		Util::Callback<const Util::Color::Rgba &> onColorLight;
		Util::Callback<const Util::Color::Rgba &> onColorFog;
		Util::Callback<uint>					  onShadingMode;
		Util::Callback<float>					  onSpecularFactor;
		Util::Callback<float>					  onShininess;
		Util::Callback<uint>					  onToonSteps;
		Util::Callback<float>					  onFogNear;
		Util::Callback<float>					  onFogFar;
		Util::Callback<float>					  onFogDensity;
		Util::Callback<const Util::Color::Rgba &> onColorOutline;
		Util::Callback<float>					  onOutlineSensitivity;
		Util::Callback<uint>					  onOutlineThickness;
		Util::Callback<const Util::Color::Rgba &> onColorSelection;

		// TODO: callback for each modifiable value?
	};

} // namespace VTX::Renderer::Proxy

#endif
