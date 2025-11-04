#ifndef __VTX_APP_SETTINGS__
#define __VTX_APP_SETTINGS__

#include <string>

namespace VTX::App::Settings
{
	namespace Camera
	{
		inline const std::string NEAR_CLIP_KEY	= "CAMERA_NEAR_CLIP";
		inline const std::string FAR_CLIP_KEY	= "CAMERA_FAR_CLIP";
		inline const std::string FOV_KEY		= "CAMERA_FOV";
		inline const std::string PROJECTION_KEY = "CAMERA_PROJECTION";
	} // namespace Camera

	namespace Controller
	{
		inline const std::string TRANSLATION_SPEED_KEY	 = "CONTROLLER_TRANSLATION_SPEED";
		inline const std::string ACCELERATION_FACTOR_KEY = "ACCELERATION_FACTOR_KEY	";
		inline const std::string DECELERATION_FACTOR_KEY = "DECELERATION_FACTOR_KEY	";
		inline const std::string ROTATION_SPEED_KEY		 = "CONTROLLER_ROTATION_SPEED";
		inline const std::string INVERT_Y_KEY			 = "INVERT_Y";
		inline const std::string ELASTICITY_ACTIVE_KEY	 = "ELASTICITY_ACTIVE";
		inline const std::string ELASTICITY_FACTOR_KEY	 = "ELASTICITY_FACTOR";
	} // namespace Controller

	namespace Selection
	{
		// inline const std::string MOLECULE_GRANULARITY_KEY = "MOLECULE_GRANULARITY";
		//  inline const App::Selection::Granularity MOLECULE_GRANULARITY_DEFAULT =
		//  App::Selection::Granularity::RESIDUE;
	} // namespace Selection

	namespace Network
	{
		inline const std::string URL_API_KEY	 = "URL_API";
		inline const std::string URL_API_DEFAULT = "URL_API";
	} // namespace Network

	void initSettings();
} // namespace VTX::App::Settings

#endif
