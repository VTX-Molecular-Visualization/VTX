#include "app/application/setting.hpp"

namespace VTX::App::Application
{
	// Camera
	const float								   Setting::CAMERA_NEAR_DEFAULT = 0.00f;
	const float								   Setting::CAMERA_NEAR_MIN		= 0.00f;
	const float								   Setting::CAMERA_NEAR_MAX		= 1e4f;
	const float								   Setting::CAMERA_FAR_DEFAULT	= 1e4f;
	const float								   Setting::CAMERA_FAR_MIN		= 0.01f;
	const float								   Setting::CAMERA_FAR_MAX		= 1e4f;
	const float								   Setting::CAMERA_FOV_DEFAULT	= 45.f;
	const float								   Setting::CAMERA_FOV_MIN		= 10.f;
	const float								   Setting::CAMERA_FOV_MAX		= 90.f;
	const Component::Render::CAMERA_PROJECTION Setting::CAMERA_PERSPECTIVE_DEFAULT
		= Component::Render::CAMERA_PROJECTION::PERSPECTIVE;
} // namespace VTX::App::Application
