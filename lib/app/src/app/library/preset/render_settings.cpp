#include "app/library/preset/render_settings.hpp"

namespace VTX::App::Library::Preset
{
	RenderSettings::RenderSettings()
	{
		using namespace Renderer::Settings;

		// Set default values.
		_data.shadingMode		 = SHADING_MODE_DEFAULT;
		_data.colorLight		 = COLOR_LIGHT_DEFAULT;
		_data.colorBackground	 = COLOR_BACKGROUND_DEFAULT;
		_data.specularFactor	 = SPECULAR_FACTOR_DEFAULT;
		_data.shininess			 = SHININESS_DEFAULT;
		_data.toonSteps			 = TOON_STEPS_DEFAULT;
		_data.activeSSAO		 = ACTIVE_SSAO_DEFAULT;
		_data.ssaoIntensity		 = SSAO_INTENSITY_DEFAULT;
		_data.blurSize			 = BLUR_SIZE_DEFAULT;
		_data.activeOutline		 = ACTIVE_OUTLINE_DEFAULT;
		_data.colorOutline		 = COLOR_OUTLINE_DEFAULT;
		_data.outlineSensitivity = OUTLINE_SENSITIVITY_DEFAULT;
		_data.outlineThickness	 = OUTLINE_THICKNESS_DEFAULT;
		_data.activeFog			 = ACTIVE_FOG_DEFAULT;
		_data.colorFog			 = COLOR_FOG_DEFAULT;
		_data.fogNear			 = FOG_NEAR_DEFAULT;
		_data.fogFar			 = FOG_FAR_DEFAULT;
		_data.fogDensity		 = FOG_DENSITY_DEFAULT;
		_data.activeSelection	 = ACTIVE_SELECTION_DEFAULT;
		_data.colorSelection	 = COLOR_SELECTION_DEFAULT;
	}

	void RenderSettings::save() {}
	void RenderSettings::load() {}
} // namespace VTX::App::Library::Preset
