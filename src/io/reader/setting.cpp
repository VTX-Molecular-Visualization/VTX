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
				VTX_ACTION( new Action::Setting::ChangeTheme( json[ "THEME" ] ) );
				VTX_ACTION( new Action::Setting::ChangeDisplayMode( json[ "SYMBOL_DISPLAY_MODE" ] ) );

				VTX_ACTION( new Action::Setting::ActiveRenderer( json[ "ACTIVE_RENDERER" ] ) );
				VTX_ACTION( new Action::Setting::ChangeRepresentation( json[ "REPRESENTATION" ] ) );
				VTX_ACTION( new Action::Setting::ChangeAtomsRadius( json[ "ATOMS_RADIUS" ] ) );
				VTX_ACTION( new Action::Setting::ChangeBondsRadius( json[ "BONDS_RADIUS" ] ) );
				VTX_ACTION( new Action::Setting::ChangeColorMode( json[ "COLOR_MODE" ] ) );
				VTX_ACTION( new Action::Setting::ChangeShading( json[ "SHADING" ] ) );
				VTX_ACTION( new Action::Setting::ActiveVerticalSync( json[ "ACTIVE_VSYNC" ] ) );
				VTX_ACTION( new Action::Setting::ActiveAO( json[ "ACTIVE_AO" ] ) );
				VTX_ACTION( new Action::Setting::ChangeAOIntensity( json[ "AO_INTENSITY" ] ) );
				VTX_ACTION( new Action::Setting::ChangeAOBlurSize( json[ "AO_BLUR_SIZE" ] ) );
				VTX_ACTION( new Action::Setting::ActiveAA( json[ "ACTIVE_AA" ] ) );
				VTX_ACTION( new Action::Setting::ActiveOutline( json[ "ACTIVE_OUTLINE" ] ) );
				//{ "OUTLINE_COLOR", VTX_SETTING().outlineColor },
				//{ "BACKGROUND_COLOR", VTX_SETTING().backgroundColor },

				VTX_ACTION( new Action::Setting::ChangeCameraClip( json[ "CAMERA_NEAR" ], json[ "CAMERA_FAR" ] ) );
				VTX_ACTION( new Action::Setting::ChangeCameraFov( json[ "CAMERA_FOV" ] ) );
				VTX_ACTION( new Action::Setting::ChangeCameraProjection( json[ "CAMERA_PERSPECTIVE" ] ) );

				VTX_ACTION( new Action::Setting::ChangeTranslationSpeed( json[ "CONTROLLER_TRANSLATION_SPEED" ] ) );
				VTX_ACTION(
					new Action::Setting::ChangeTranslationFactorSpeed( json[ "CONTROLLER_TRANSLATION_FACTOR" ] ) );
				VTX_ACTION( new Action::Setting::ChangeRotationSpeed( json[ "CONTROLLER_ROTATION_SPEED" ] ) );
				VTX_ACTION( new Action::Setting::ActiveYAxisInversion( json[ "CONTROLLER_Y_AXIS_INVERTED" ] ) );

				//{ "AUTO_ROTATE_SPEED", VTX_SETTING().autoRotationSpeed }
			} // namespace Reader
		}	  // namespace Reader
	}		  // namespace IO
} // namespace VTX
