#ifndef __VTX_CORE_STRUCT_RENDER_SETTINGS__
#define __VTX_CORE_STRUCT_RENDER_SETTINGS__

#include <util/color/rgba.hpp>
#include <util/constants.hpp>

namespace VTX::Core::Struct
{
	// TODO: move default layout to... ?
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
	};
} // namespace VTX::Core::Struct

#endif
