#ifndef __VTX_RENDERER_PROXY_RENDER_SETTINGS__
#define __VTX_RENDERER_PROXY_RENDER_SETTINGS__

#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{
	// TODO: to handle multiples render settings in the same scene.
	struct RenderSettings
	{
		std::optional<float> ssaoIntensity;

		std::optional<Vec2i> blurXDirection;
		std::optional<float> blurXSize;

		std::optional<Vec2i> blurYDirection;
		std::optional<float> blurYSize;

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

		std::optional<Util::Color::Rgba> colorOutline;
		std::optional<float>			 outlineSensitivity;
		std::optional<float>			 outlineThickness;

		Util::Color::Rgba colorSelection;

		// TODO: callback for each modifiable value?
	};

} // namespace VTX::Renderer::Proxy

#endif
