#include "setting.hpp"
#include "renderer/gl/pass/shading.hpp"

namespace VTX
{
	// UI.
	const Style::THEME				 Setting::THEME_DEFAULT				  = Style::THEME::DARK;
	const Style::SYMBOL_DISPLAY_MODE Setting::SYMBOL_DISPLAY_MODE_DEFAULT = Style::SYMBOL_DISPLAY_MODE::SHORT;
	const int						 Setting::WINDOW_WIDTH_DEFAULT		  = 1280;
	const int						 Setting::WINDOW_HEIGHT_DEFAULT		  = 720;

	// Rendering.
	const bool					  Setting::ACTIVE_RENDERER_DEFAULT	= true;
	const Generic::REPRESENTATION Setting::REPRESENTATION_DEFAULT	= Generic::REPRESENTATION::VAN_DER_WAALS;
	const float					  Setting::ATOMS_RADIUS_DEFAULT		= 0.4f;
	const float					  Setting::ATOMS_RADIUS_MIN			= 0.f;
	const float					  Setting::ATOMS_RADIUS_MAX			= 1.f;
	const float					  Setting::BONDS_RADIUS_DEFAULT		= 0.15f;
	const float					  Setting::BONDS_RADIUS_MIN			= 0.f;
	const float					  Setting::BONDS_RADIUS_MAX			= 1.f;
	const Generic::COLOR_MODE	  Setting::COLOR_MODE_DEFAULT		= Generic::COLOR_MODE::CHAIN;
	const Renderer::SHADING		  Setting::SHADING_DEFAULT			= Renderer::SHADING::DIFFUSE;
	const bool					  Setting::ACTIVE_VSYNC_DEFAULT		= true;
	const bool					  Setting::ACTIVE_AO_DEFAULT		= true;
	const int					  Setting::AO_INTENSITY_DEFAULT		= 5;
	const int					  Setting::AO_INTENSITY_MIN			= 1;
	const int					  Setting::AO_INTENSITY_MAX			= 20;
	const int					  Setting::AO_BLUR_SIZE_DEFAULT		= 17;
	const int					  Setting::AO_BLUR_SIZE_MIN			= 1;
	const int					  Setting::AO_BLUR_SIZE_MAX			= 99;
	const bool					  Setting::ACTIVE_OUTLINE_DEFAULT	= false;
	const bool					  Setting::ACTIVE_FOG_DEFAULT		= false;
	const float					  Setting::FOG_NEAR_DEFAULT			= 400.f;
	const float					  Setting::FOG_NEAR_MIN				= 0.f;
	const float					  Setting::FOG_NEAR_MAX				= 1000.f;
	const float					  Setting::FOG_FAR_DEFAULT			= 600.f;
	const float					  Setting::FOG_FAR_MIN				= 0.f;
	const float					  Setting::FOG_FAR_MAX				= 1000.f;
	const float					  Setting::FOG_DENSITY_DEFAULT		= 0.8f;
	const bool					  Setting::ACTIVE_AA_DEFAULT		= true;
	const Color::Rgb			  Setting::OUTLINE_COLOR_DEFAULT	= Color::Rgb::WHITE;
	const Color::Rgb			  Setting::BACKGROUND_COLOR_DEFAULT = Color::Rgb::BLACK;

	// Camera.
	const float Setting::CAMERA_NEAR_DEFAULT		= 0.f;
	const float Setting::CAMERA_NEAR_MIN			= 0.f;
	const float Setting::CAMERA_NEAR_MAX			= 1e4f;
	const float Setting::CAMERA_FAR_DEFAULT			= 1e4f;
	const float Setting::CAMERA_FAR_MIN				= 0.f;
	const float Setting::CAMERA_FAR_MAX				= 1e4f;
	const float Setting::CAMERA_FOV_DEFAULT			= 45.f;
	const float Setting::CAMERA_FOV_MIN				= 10.f;
	const float Setting::CAMERA_FOV_MAX				= 90.f;
	const bool	Setting::CAMERA_PERSPECTIVE_DEFAULT = true;

	// Controllers.
	const float Setting::CONTROLLER_TRANSLATION_SPEED_DEFAULT  = 150.f;
	const float Setting::CONTROLLER_TRANSLATION_SPEED_MIN	   = 50.f;
	const float Setting::CONTROLLER_TRANSLATION_SPEED_MAX	   = 300.f;
	const float Setting::CONTROLLER_TRANSLATION_FACTOR_DEFAULT = 2.0f;
	const float Setting::CONTROLLER_TRANSLATION_FACTOR_MIN	   = 1.0f;
	const float Setting::CONTROLLER_TRANSLATION_FACTOR_MAX	   = 5.0f;
	const float Setting::CONTROLLER_ROTATION_SPEED_DEFAULT	   = 0.005f;
	const float Setting::CONTROLLER_ROTATION_SPEED_MIN		   = 0.001f;
	const float Setting::CONTROLLER_ROTATION_SPEED_MAX		   = 0.01f;
	const bool	Setting::CONTROLLER_Y_AXIS_INVERTED			   = false;
	const float Setting::CONTROLLER_ELASTICITY_FACTOR		   = 4.0f;
	const float Setting::CONTROLLER_ELASTICITY_THRESHOLD	   = 1e-4f;

	// Auto rotate.
	const float Setting::AUTO_ROTATE_SPEED_DEFAULT = 0.0f;
	const float Setting::AUTO_ROTATE_SPEED_MIN	   = 0.0f;
	const float Setting::AUTO_ROTATE_SPEED_MAX	   = 1.0f;

	// Video.
	const float Setting::PATH_DURATION_DEFAULT = 5.f;
	const uint	Setting::VIDEO_FPS_DEFAULT	   = 60;

	// Misc.
	const int  Setting::CONSOLE_SIZE	   = 80;
	const uint Setting::ACTION_BUFFER_SIZE = 10;

} // namespace VTX
