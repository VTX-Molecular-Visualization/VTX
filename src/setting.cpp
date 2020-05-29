#include "setting.hpp"

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
			extern bool					  isActive		  = true;
			View::MOLECULE_REPRESENTATION representation  = View::MOLECULE_REPRESENTATION::VAN_DER_WAALS;
			View::MOLECULE_COLOR_MODE	  colorMode		  = View::MOLECULE_COLOR_MODE::CHAIN;
			Renderer::MODE				  mode			  = Renderer::MODE::GL;
			Renderer::SHADING			  shading		  = Renderer::SHADING::BLINN_PHONG;
			bool						  useVSync		  = VSYNC_DEFAULT;
			bool						  useSSAO		  = SSAO_DEFAULT;
			float						  aoRadius		  = RENDERER_AO_RADIUS_DEFAULT;
			int							  aoIntensity	  = RENDERER_AO_INTENSITY_DEFAULT;
			float						  aoFactor		  = RENDERER_AO_FACTOR_DEFAULT;
			int							  aoBlurSize	  = RENDERER_AO_BLUR_SIZE_DEFAULT;
			int							  aoBlurSharpness = RENDERER_AO_BLUR_SHARPNESS_DEFAULT;
			bool						  useContourLines = CONTOUR_LINES_DEFAULT;
			bool						  useAA			  = AA_DEFAULT;

			// Camera.
			float camNear = CAMERA_NEAR_DEFAULT;
			float camFar  = CAMERA_FAR_DEFAULT;
			float camFov  = CAMERA_FOV_DEFAULT;

			Color::Rgb contourLineColor = Color::Rgb::WHITE;
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
