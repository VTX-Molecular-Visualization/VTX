#ifndef __VTX_APP_INTERNAL_APPLICATION_SETTINGS__
#define __VTX_APP_INTERNAL_APPLICATION_SETTINGS__

#include "app/application/selection/molecule_granularity.hpp"
#include "app/application/settings/settings.hpp"
#include "app/component/render/camera.hpp"
#include <string>

namespace VTX::App::Internal::Application::Settings
{
	void initSettings( App::Application::Settings::Settings & p_setting );

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

	namespace Selection
	{
		const std::string							   MOLECULE_GRANULARITY_KEY = "MOLECULE_GRANULARITY";
		const App::Application::Selection::Granularity MOLECULE_GRANULARITY_DEFAULT
			= App::Application::Selection::Granularity::ATOM;
	} // namespace Selection
} // namespace VTX::App::Internal::Application::Settings

#endif
