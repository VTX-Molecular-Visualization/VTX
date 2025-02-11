#ifndef __VTX_APP_SETTINGS__
#define __VTX_APP_SETTINGS__

#include "app/component/render/camera.hpp"
#include "app/selection/system_granularity.hpp"
#include <string>

namespace VTX::App::Settings
{
	namespace Camera
	{
		const std::string							NEAR_CLIP_KEY  = "CAMERA_NEAR_CLIP";
		const float									NEAR_DEFAULT   = 0.00f;
		const float									NEAR_MIN	   = 0.00f;
		const float									NEAR_MAX	   = 1e4f;
		const std::string							FAR_CLIP_KEY   = "CAMERA_FAR_CLIP";
		const float									FAR_DEFAULT	   = 1e4f;
		const float									FAR_MIN		   = 0.01f;
		const float									FAR_MAX		   = 1e4f;
		const std::string							FOV_KEY		   = "CAMERA_FOV";
		const float									FOV_DEFAULT	   = 45.f;
		const float									FOV_MIN		   = 10.f;
		const float									FOV_MAX		   = 90.f;
		const std::string							PROJECTION_KEY = "CAMERA_PROJECTION";
		const Component::Render::Camera::PROJECTION PROJECTION_DEFAULT
			= Component::Render::Camera::PROJECTION::PERSPECTIVE;
	} // namespace Camera

	namespace Controller
	{
		inline const std::string TRANSLATION_SPEED_KEY	   = "CONTROLLER_TRANSLATION_SPEED";
		inline const float		 TRANSLATION_SPEED_DEFAULT = 150.f;
		inline const float		 TRANSLATION_SPEED_MIN	   = 50.f;
		inline const float		 TRANSLATION_SPEED_MAX	   = 300.f;

		inline const std::string ACCELERATION_FACTOR_KEY	 = "ACCELERATION_FACTOR_KEY	";
		inline const float		 ACCELERATION_FACTOR_DEFAULT = 2.0f;
		inline const float		 ACCELERATION_FACTOR_MIN	 = 1.0f;
		inline const float		 ACCELERATION_FACTOR_MAX	 = 20.0f;
		inline const std::string DECELERATION_FACTOR_KEY	 = "DECELERATION_FACTOR_KEY	";
		inline const float		 DECELERATION_FACTOR_DEFAULT = 10.0f;
		inline const float		 DECELERATION_FACTOR_MIN	 = 1.0f;
		inline const float		 DECELERATION_FACTOR_MAX	 = 50.0f;

		inline const std::string ROTATION_SPEED_KEY		= "CONTROLLER_ROTATION_SPEED";
		inline const float		 ROTATION_SPEED_DEFAULT = 0.005f;
		inline const float		 ROTATION_SPEED_MIN		= 0.001f;
		inline const float		 ROTATION_SPEED_MAX		= 0.01f;

		inline const std::string INVERT_Y_KEY	  = "INVERT_Y";
		inline const bool		 INVERT_Y_DEFAULT = false;

		inline const std::string ELASTICITY_ACTIVE_KEY	   = "ELASTICITY_ACTIVE";
		inline const bool		 ELASTICITY_ACTIVE_DEFAULT = true;

		inline const std::string ELASTICITY_FACTOR_KEY	   = "ELASTICITY_FACTOR";
		inline const float		 ELASTICITY_FACTOR_DEFAULT = 6.0f;
		inline const float		 ELASTICITY_FACTOR_MIN	   = 1.0f;
		inline const float		 ELASTICITY_FACTOR_MAX	   = 40.0f;
	} // namespace Controller

	namespace Selection
	{
		const std::string							   MOLECULE_GRANULARITY_KEY = "MOLECULE_GRANULARITY";
		const App::Selection::Granularity MOLECULE_GRANULARITY_DEFAULT
			= App::Selection::Granularity::RESIDUE;
	} // namespace Selection

	namespace Network
	{
		const std::string URL_API_KEY	  = "URL_API";
		const std::string URL_API_DEFAULT = "URL_API";
	} // namespace Network

	void initSettings();
} // namespace VTX::App::Settings

#endif
