#include "setting.hpp"
#include "renderer/base_renderer.hpp"
#include "ui/user_interface.hpp"

namespace VTX
{
	namespace Setting
	{
		namespace UI
		{
			Setting::UI::THEME	theme			  = Setting::UI::THEME::DARK;
			SYMBOL_DISPLAY_MODE symbolDisplayMode = SYMBOL_DISPLAY_MODE::SHORT;
		}; // namespace UI

		namespace Rendering
		{
			View::MOLECULE_REPRESENTATION representation = View::MOLECULE_REPRESENTATION::VAN_DER_WAALS;
			View::MOLECULE_COLOR_MODE	  colorMode		 = View::MOLECULE_COLOR_MODE::CHAIN;
			Renderer::MODE				  mode			 = Renderer::MODE::DEFERRED;
			Renderer::SHADING			  shading		 = Renderer::SHADING::LAMBERT;
			bool						  useAA			 = false;
			bool						  useSSAO		 = true;
			float						  aoRadius		 = RENDERER_AO_RADIUS_DEFAULT;
			int							  aoIntensity	 = RENDERER_AO_INTENSITY_DEFAULT;
			int							  aoBlurSize	 = RENDERER_AO_BLUR_SIZE_DEFAULT;
		}; // namespace Rendering

		namespace Controller
		{
			bool  yAxisInverted = false;
			Vec3f autoRotateSpeed
				= Vec3f( AUTO_ROTATE_X_SPEED_DEFAULT, AUTO_ROTATE_Y_SPEED_DEFAULT, AUTO_ROTATE_Z_SPEED_DEFAULT );
			float translationSpeed		 = CONTROLLER_TRANSLATION_SPEED_DEFAULT;
			float translationFactorSpeed = CONTROLLER_TRANSLATION_FACTOR_DEFAULT;
			float rotationSpeed			 = CONTROLLER_ROTATION_SPEED_DEFAULT;
		} // namespace Controller
	}	  // namespace Setting
} // namespace VTX
