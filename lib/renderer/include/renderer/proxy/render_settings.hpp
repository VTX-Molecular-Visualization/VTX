#ifndef __VTX_RENDERER_PROXY_RENDER_SETTINGS__
#define __VTX_RENDERER_PROXY_RENDER_SETTINGS__

#include "renderer/settings.hpp"
#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{
	// TODO: to handle multiples render settings in the same scene.

	/**
	 * @brief Render settings data provider.
	 */
	struct RenderSettings
	{
		// Shading.
		uint			  shadingMode;
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

		template<E_RENDER_SETTINGS S, typename T>
		Util::Callback<const T> & onChange()
		{
			static Util::Callback<const T> callback;
			return callback;
		}

		// TODO: callback for each modifiable value?
	};

} // namespace VTX::Renderer::Proxy

#endif
