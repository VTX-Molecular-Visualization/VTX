#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <renderer/facade.hpp>
#include <util/constants.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Scene
{

	Camera::Camera()
	{
		auto & settings = SETTINGS();
		near			= settings.getValuePtr<float>( Settings::Camera::NEAR_CLIP_KEY );
		far				= settings.getValuePtr<float>( Settings::Camera::FAR_CLIP_KEY );
		fov				= settings.getValuePtr<float>( Settings::Camera::FOV_KEY );
		projection		= settings.getValuePtr<PROJECTION>( Settings::Camera::PROJECTION_KEY );
	}

	Mat4f Camera::computeProjectionMatrix() const
	{
		using namespace Util;

		switch ( *projection )
		{
		case PROJECTION::PERSPECTIVE:
		{
			return Math::perspective(
				Math::radians( *fov ), float( screenWidth ) / float( screenHeight ), *near, *far
			);
		}

		case PROJECTION::ORTHOGRAPHIC:
		{
			const float halfHeight = screenHeight * 0.5f;
			const float halfWidth  = screenWidth * 0.5f;
			return Math::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, *near, *far );
		}

		default:
		{
			assert( false );
			return MAT4F_ID;
		}
		}
	}

} // namespace VTX::App::Scene
