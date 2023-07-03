#ifndef __VTX_APP_APPLICATION_SETTING__
#define __VTX_APP_APPLICATION_SETTING__

#include "app/component/render/enum_camera.hpp"

namespace VTX::App::Application
{
	struct Setting
	{
		// Camera
		static const float								  CAMERA_NEAR_DEFAULT;
		static const float								  CAMERA_NEAR_MIN;
		static const float								  CAMERA_NEAR_MAX;
		static const float								  CAMERA_FAR_DEFAULT;
		static const float								  CAMERA_FAR_MIN;
		static const float								  CAMERA_FAR_MAX;
		static const float								  CAMERA_FOV_DEFAULT;
		static const float								  CAMERA_FOV_MIN;
		static const float								  CAMERA_FOV_MAX;
		static const Component::Render::CAMERA_PROJECTION CAMERA_PERSPECTIVE_DEFAULT;

		// Camera
		float								 cameraNearClip	  = CAMERA_NEAR_DEFAULT;
		float								 cameraFarClip	  = CAMERA_FAR_DEFAULT;
		float								 cameraFov		  = CAMERA_FOV_DEFAULT;
		Component::Render::CAMERA_PROJECTION cameraProjection = CAMERA_PERSPECTIVE_DEFAULT;
	};

} // namespace VTX::App::Application
#endif
