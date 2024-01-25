#include "app/internal/application/settings.hpp"
#include "app/component/render/enum_camera.hpp"

namespace VTX::App::Internal::Application::Settings
{
	void initSettings( App::Application::Settings & p_settings )
	{
		p_settings.referenceSetting( Camera::NEAR_CLIP_KEY, Camera::NEAR_DEFAULT );
		p_settings.referenceSetting( Camera::FAR_CLIP_KEY, Camera::FAR_DEFAULT );
		p_settings.referenceSetting( Camera::FOV_KEY, Camera::FOV_DEFAULT );
		p_settings.referenceSetting( Camera::PROJECTION_KEY, Camera::PROJECTION_DEFAULT );
	}
} // namespace VTX::App::Internal::Application::Settings
