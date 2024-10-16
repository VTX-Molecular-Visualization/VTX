#ifndef __VTX_CORE_STRUCT_RENDER_SETTINGS__
#define __VTX_CORE_STRUCT_RENDER_SETTINGS__

#include <util/color/rgba.hpp>
#include <util/constants.hpp>

namespace VTX::Core::Struct
{
	// TODO: move default layout to... ?
	struct RenderSettings
	{
		float			  ssaoIntensity		 = 5.f;
		float			  blurSize			 = 17.f;
		Util::Color::Rgba colorBackground	 = COLOR_BLACK;
		Util::Color::Rgba colorLight		 = COLOR_WHITE;
		Util::Color::Rgba colorFog			 = COLOR_GREY;
		uint			  shadingMode		 = 0;
		float			  specularFactor	 = 0.4f;
		float			  shininess			 = 32.f;
		uint			  toonSteps			 = 4;
		float			  fogNear			 = 30.f;
		float			  fogFar			 = 80.f;
		float			  fogDensity		 = 0.f;
		Util::Color::Rgba colorOutline		 = COLOR_WHITE;
		float			  outlineSensitivity = 1.f;
		uint			  outlineThickness	 = 1;
		Util::Color::Rgba colorSelection	 = Util::Color::Rgba( 45, 243, 26 );
	};
} // namespace VTX::Core::Struct

#endif
