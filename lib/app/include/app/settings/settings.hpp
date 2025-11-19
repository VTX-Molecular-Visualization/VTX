#ifndef __VTX_APP_SETTINGS__
#define __VTX_APP_SETTINGS__

#include <string>

namespace VTX::App::Settings
{
	namespace Camera
	{
		constexpr std::string_view NEAR_CLIP_KEY  = "CAMERA_NEAR_CLIP";
		constexpr std::string_view FAR_CLIP_KEY	  = "CAMERA_FAR_CLIP";
		constexpr std::string_view FOV_KEY		  = "CAMERA_FOV";
		constexpr std::string_view PROJECTION_KEY = "CAMERA_PROJECTION";
	} // namespace Camera

	namespace Controller
	{
		constexpr std::string_view TRANSLATION_SPEED_KEY   = "CONTROLLER_TRANSLATION_SPEED";
		constexpr std::string_view ACCELERATION_FACTOR_KEY = "ACCELERATION_FACTOR_KEY	";
		constexpr std::string_view DECELERATION_FACTOR_KEY = "DECELERATION_FACTOR_KEY	";
		constexpr std::string_view ROTATION_SPEED_KEY	   = "CONTROLLER_ROTATION_SPEED";
		constexpr std::string_view INVERT_Y_KEY			   = "INVERT_Y";
		constexpr std::string_view ELASTICITY_ACTIVE_KEY   = "ELASTICITY_ACTIVE";
		constexpr std::string_view ELASTICITY_FACTOR_KEY   = "ELASTICITY_FACTOR";
	} // namespace Controller

	namespace Selection
	{
		// inline const std::string MOLECULE_GRANULARITY_KEY = "MOLECULE_GRANULARITY";
		//  inline const App::Selection::Granularity MOLECULE_GRANULARITY_DEFAULT =
		//  App::Selection::Granularity::RESIDUE;
	} // namespace Selection

	namespace Network
	{
		constexpr std::string_view URL_API_KEY	   = "URL_API";
		constexpr std::string_view URL_API_DEFAULT = "URL_API";
	} // namespace Network

	void initSettings();
} // namespace VTX::App::Settings

#endif
