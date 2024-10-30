#ifndef __VTX_CORE_STRUCT_RENDER_SETTINGS__
#define __VTX_CORE_STRUCT_RENDER_SETTINGS__

#include <util/color/rgba.hpp>
#include <util/constants.hpp>

namespace VTX::Core::Struct
{
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
	};
} // namespace VTX::Core::Struct

#endif
