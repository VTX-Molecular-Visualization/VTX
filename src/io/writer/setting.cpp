#include "setting.hpp"
#include "util/filesystem.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			void Setting::writeFile( const Path & p_path, const VTX::Setting & p_data )
			{
				nlohmann::json json = {
					{ "THEME", VTX_SETTING().theme },
					{ "SYMBOL_DISPLAY_MODE", VTX_SETTING().theme },

					{ "ACTIVE_RENDERER", VTX_SETTING().activeRenderer },
					{ "REPRESENTATION", VTX_SETTING().representation },
					{ "ATOMS_RADIUS", VTX_SETTING().atomsRadius },
					{ "BONDS_RADIUS", VTX_SETTING().bondsRadius },
					{ "COLOR_MODE", VTX_SETTING().colorMode },
					{ "SHADING", VTX_SETTING().shading },
					{ "ACTIVE_VSYNC", VTX_SETTING().activeVSync },
					{ "ACTIVE_AO", VTX_SETTING().activeAO },
					{ "AO_INTENSITY", VTX_SETTING().aoIntensity },
					{ "AO_BLUR_SIZE", VTX_SETTING().aoBlurSize },
					{ "ACTIVE_OUTLINE", VTX_SETTING().activeOutline },
					{ "OUTLINE_COLOR", VTX_SETTING().outlineColor.toStdVector() },
					{ "ACTIVE_FOG", VTX_SETTING().activeFog },
					{ "FOG_NEAR", VTX_SETTING().fogNear },
					{ "FOG_FAR", VTX_SETTING().fogFar },
					{ "FOG_DENSITY", VTX_SETTING().fogDensity },
					{ "ACTIVE_AA", VTX_SETTING().activeAA },
					{ "BACKGROUND_COLOR", VTX_SETTING().backgroundColor.toStdVector() },

					{ "CAMERA_NEAR", VTX_SETTING().cameraNear },
					{ "CAMERA_FAR", VTX_SETTING().cameraFar },
					{ "CAMERA_FOV", VTX_SETTING().cameraFov },
					{ "CAMERA_PERSPECTIVE", VTX_SETTING().cameraPerspective },

					{ "CONTROLLER_TRANSLATION_SPEED", VTX_SETTING().translationSpeed },
					{ "CONTROLLER_TRANSLATION_FACTOR", VTX_SETTING().translationFactorSpeed },
					{ "CONTROLLER_ROTATION_SPEED", VTX_SETTING().rotationSpeed },
					{ "CONTROLLER_Y_AXIS_INVERTED", VTX_SETTING().yAxisInverted },

					//{ "AUTO_ROTATE_SPEED", VTX_SETTING().autoRotationSpeed }
				};

				std::ofstream os( p_path );
				os << std::setw( 4 ) << json << std::endl;
			}

		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
