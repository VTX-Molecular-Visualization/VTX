#include "setting.hpp"
#include "action/setting.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void Setting::readFile( const Path & p_path, VTX::Setting & p_data )
			{
				std::ifstream  is( p_path );
				nlohmann::json json;
				is >> json;

				// TODO: check values before update, or force all values?

				// Set new values with actions.
				VTX_ACTION( new Action::Setting::ChangeTheme( json[ "THEME" ].get<Style::THEME>() ) );
				VTX_ACTION( new Action::Setting::ChangeDisplayMode(
					json[ "SYMBOL_DISPLAY_MODE" ].get<Style::SYMBOL_DISPLAY_MODE>() ) );

				VTX_ACTION( new Action::Setting::ActiveRenderer( json[ "ACTIVE_RENDERER" ].get<bool>() ) );
				VTX_ACTION( new Action::Setting::ChangeRepresentation(
					json[ "REPRESENTATION" ].get<Generic::REPRESENTATION>() ) );
				VTX_ACTION( new Action::Setting::ChangeAtomsRadius( json[ "ATOMS_RADIUS" ].get<float>() ) );
				VTX_ACTION( new Action::Setting::ChangeBondsRadius( json[ "BONDS_RADIUS" ].get<float>() ) );
				VTX_ACTION( new Action::Setting::ChangeColorMode( json[ "COLOR_MODE" ].get<Generic::COLOR_MODE>() ) );
				VTX_ACTION( new Action::Setting::ChangeShading( json[ "SHADING" ].get<Renderer::SHADING>() ) );
				VTX_ACTION( new Action::Setting::ActiveVerticalSync( json[ "ACTIVE_VSYNC" ].get<bool>() ) );
				VTX_ACTION( new Action::Setting::ActiveAO( json[ "ACTIVE_AO" ].get<bool>() ) );
				VTX_ACTION( new Action::Setting::ChangeAOIntensity( json[ "AO_INTENSITY" ].get<int>() ) );
				VTX_ACTION( new Action::Setting::ChangeAOBlurSize( json[ "AO_BLUR_SIZE" ].get<int>() ) );
				VTX_ACTION( new Action::Setting::ActiveAA( json[ "ACTIVE_AA" ].get<bool>() ) );
				VTX_ACTION( new Action::Setting::ActiveOutline( json[ "ACTIVE_OUTLINE" ].get<bool>() ) );
				//{ "OUTLINE_COLOR", VTX_SETTING().outlineColor },
				//{ "BACKGROUND_COLOR", VTX_SETTING().backgroundColor },

				VTX_ACTION(
					new Action::Setting::ChangeCameraClip( json[ "CAMERA_NEAR" ], json[ "CAMERA_FAR" ].get<float>() ) );
				VTX_ACTION( new Action::Setting::ChangeCameraFov( json[ "CAMERA_FOV" ].get<float>() ) );
				VTX_ACTION( new Action::Setting::ChangeCameraProjection( json[ "CAMERA_PERSPECTIVE" ].get<bool>() ) );

				VTX_ACTION( new Action::Setting::ChangeTranslationSpeed(
					json[ "CONTROLLER_TRANSLATION_SPEED" ].get<float>() ) );
				VTX_ACTION( new Action::Setting::ChangeTranslationFactorSpeed(
					json[ "CONTROLLER_TRANSLATION_FACTOR" ].get<float>() ) );
				VTX_ACTION(
					new Action::Setting::ChangeRotationSpeed( json[ "CONTROLLER_ROTATION_SPEED" ].get<float>() ) );
				VTX_ACTION(
					new Action::Setting::ActiveYAxisInversion( json[ "CONTROLLER_Y_AXIS_INVERTED" ].get<bool>() ) );

				//{ "AUTO_ROTATE_SPEED", VTX_SETTING().autoRotationSpeed }
			} // namespace Reader
		}	  // namespace Reader
	}		  // namespace IO
} // namespace VTX
