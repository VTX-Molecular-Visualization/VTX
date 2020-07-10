#include "setting.hpp"
#include "action/setting.hpp"
#include "renderer/gl/pass/shading.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	// UI.
	const Style::THEME				 Setting::THEME_DEFAULT				  = Style::THEME::DARK;
	const Style::SYMBOL_DISPLAY_MODE Setting::SYMBOL_DISPLAY_MODE_DEFAULT = Style::SYMBOL_DISPLAY_MODE::SHORT;
	const int						 Setting::WINDOW_WIDTH_DEFAULT		  = 1280;
	const int						 Setting::WINDOW_HEIGHT_DEFAULT		  = 720;

	// Rendering.
	const bool					  Setting::ACTIVE_RENDERER_DEFAULT	= true;
	const Color::Rgb			  Setting::BACKGROUND_COLOR_DEFAULT = Color::Rgb::BLACK;
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
	const Color::Rgb			  Setting::OUTLINE_COLOR_DEFAULT	= Color::Rgb::WHITE;
	const bool					  Setting::ACTIVE_FOG_DEFAULT		= false;
	const float					  Setting::FOG_NEAR_DEFAULT			= 400.f;
	const float					  Setting::FOG_NEAR_MIN				= 0.f;
	const float					  Setting::FOG_NEAR_MAX				= 1000.f;
	const float					  Setting::FOG_FAR_DEFAULT			= 600.f;
	const float					  Setting::FOG_FAR_MIN				= 0.f;
	const float					  Setting::FOG_FAR_MAX				= 1000.f;
	const float					  Setting::FOG_DENSITY_DEFAULT		= 0.8f;
	const Color::Rgb			  Setting::FOG_COLOR_DEFAULT		= Color::Rgb::WHITE;
	const bool					  Setting::ACTIVE_AA_DEFAULT		= true;
	const Color::Rgb			  Setting::LIGHT_COLOR_DEFAULT		= Color::Rgb::WHITE;

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
	const uint	Setting::VIDEO_CRF_DEFAULT	   = 10;

	// Misc.
	const int  Setting::CONSOLE_SIZE	   = 80;
	const uint Setting::ACTION_BUFFER_SIZE = 10;

	nlohmann::json Setting::toJson() const
	{
		return { { "THEME", theme },
				 { "SYMBOL_DISPLAY_MODE", symbolDisplayMode },

				 { "ACTIVE_RENDERER", activeRenderer },
				 { "BACKGROUND_COLOR", backgroundColor.toJson() },
				 { "REPRESENTATION", representation },
				 { "ATOMS_RADIUS", atomsRadius },
				 { "BONDS_RADIUS", bondsRadius },
				 { "COLOR_MODE", colorMode },
				 { "SHADING", shading },
				 { "ACTIVE_VSYNC", activeVSync },
				 { "ACTIVE_AO", activeAO },
				 { "AO_INTENSITY", aoIntensity },
				 { "AO_BLUR_SIZE", aoBlurSize },
				 { "ACTIVE_OUTLINE", activeOutline },
				 { "OUTLINE_COLOR", outlineColor.toJson() },
				 { "ACTIVE_FOG", activeFog },
				 { "FOG_NEAR", fogNear },
				 { "FOG_FAR", fogFar },
				 { "FOG_DENSITY", fogDensity },
				 { "FOG_COLOR", fogColor.toJson() },
				 { "ACTIVE_AA", activeAA },
				 { "LIGHT_COLOR", lightColor.toJson() },

				 { "CAMERA_NEAR", cameraNear },
				 { "CAMERA_FAR", cameraFar },
				 { "CAMERA_FOV", cameraFov },
				 { "CAMERA_PERSPECTIVE", cameraPerspective },

				 { "CONTROLLER_TRANSLATION_SPEED", translationSpeed },
				 { "CONTROLLER_TRANSLATION_FACTOR", translationFactorSpeed },
				 { "CONTROLLER_ROTATION_SPEED", rotationSpeed },
				 { "CONTROLLER_Y_AXIS_INVERTED", yAxisInverted },

				 { "AUTO_ROTATE_SPEED", Util::Math::vecToJson( autoRotationSpeed ) } };
	}

	void Setting::fromJson( nlohmann::json & p_json )
	{
		// TODO: check if value exists and if !=

		// Set new values with actions.
		VTX_ACTION( new Action::Setting::ChangeTheme( p_json[ "THEME" ].get<Style::THEME>() ) );
		VTX_ACTION( new Action::Setting::ChangeDisplayMode(
			p_json[ "SYMBOL_DISPLAY_MODE" ].get<Style::SYMBOL_DISPLAY_MODE>() ) );

		VTX_ACTION( new Action::Setting::ActiveRenderer( p_json[ "ACTIVE_RENDERER" ].get<bool>() ) );
		Color::Rgb bgColor = Color::Rgb();
		bgColor.fromJson( p_json[ "BACKGROUND_COLOR" ] );
		VTX_ACTION( new Action::Setting::ChangeBackgroundColor( bgColor ) );
		VTX_ACTION(
			new Action::Setting::ChangeRepresentation( p_json[ "REPRESENTATION" ].get<Generic::REPRESENTATION>() ) );
		VTX_ACTION( new Action::Setting::ChangeAtomsRadius( p_json[ "ATOMS_RADIUS" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ChangeBondsRadius( p_json[ "BONDS_RADIUS" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ChangeColorMode( p_json[ "COLOR_MODE" ].get<Generic::COLOR_MODE>() ) );
		VTX_ACTION( new Action::Setting::ChangeShading( p_json[ "SHADING" ].get<Renderer::SHADING>() ) );
		VTX_ACTION( new Action::Setting::ActiveVerticalSync( p_json[ "ACTIVE_VSYNC" ].get<bool>() ) );
		VTX_ACTION( new Action::Setting::ActiveAO( p_json[ "ACTIVE_AO" ].get<bool>() ) );
		VTX_ACTION( new Action::Setting::ChangeAOIntensity( p_json[ "AO_INTENSITY" ].get<int>() ) );
		VTX_ACTION( new Action::Setting::ChangeAOBlurSize( p_json[ "AO_BLUR_SIZE" ].get<int>() ) );
		VTX_ACTION( new Action::Setting::ActiveOutline( p_json[ "ACTIVE_OUTLINE" ].get<bool>() ) );
		Color::Rgb outlinColor = Color::Rgb();
		outlinColor.fromJson( p_json[ "OUTLINE_COLOR" ] );
		VTX_ACTION( new Action::Setting::ChangeOutlineColor( outlinColor ) );
		VTX_ACTION( new Action::Setting::ActiveFog( p_json[ "ACTIVE_FOG" ].get<bool>() ) );
		VTX_ACTION( new Action::Setting::ChangeFogNear( p_json[ "FOG_NEAR" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ChangeFogFar( p_json[ "FOG_FAR" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ChangeFogDensity( p_json[ "FOG_DENSITY" ].get<float>() ) );
		Color::Rgb fogColor = Color::Rgb();
		fogColor.fromJson( p_json[ "FOG_COLOR" ] );
		VTX_ACTION( new Action::Setting::ChangeFogColor( fogColor ) );
		VTX_ACTION( new Action::Setting::ActiveAA( p_json[ "ACTIVE_AA" ].get<bool>() ) );
		Color::Rgb lightColor = Color::Rgb();
		lightColor.fromJson( p_json[ "LIGHT_COLOR" ] );
		VTX_ACTION( new Action::Setting::ChangeLightColor( lightColor ) );

		VTX_ACTION(
			new Action::Setting::ChangeCameraClip( p_json[ "CAMERA_NEAR" ], p_json[ "CAMERA_FAR" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ChangeCameraFov( p_json[ "CAMERA_FOV" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ChangeCameraProjection( p_json[ "CAMERA_PERSPECTIVE" ].get<bool>() ) );

		VTX_ACTION(
			new Action::Setting::ChangeTranslationSpeed( p_json[ "CONTROLLER_TRANSLATION_SPEED" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ChangeTranslationFactorSpeed(
			p_json[ "CONTROLLER_TRANSLATION_FACTOR" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ChangeRotationSpeed( p_json[ "CONTROLLER_ROTATION_SPEED" ].get<float>() ) );
		VTX_ACTION( new Action::Setting::ActiveYAxisInversion( p_json[ "CONTROLLER_Y_AXIS_INVERTED" ].get<bool>() ) );

		VTX_ACTION( new Action::Setting::ChangeAutoRotateSpeed(
			Util::Math::jsonToVec<Vec3f>( p_json[ "AUTO_ROTATE_SPEED" ] ) ) );
	}

} // namespace VTX
