#include "app/scene/camera.hpp"
#include <renderer/facade.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Scene
{

	Mat4f Camera::computeProjectionMatrix() const
	{
		using namespace Util::Math;

		switch ( projection )
		{
		case PROJECTION::PERSPECTIVE:
		{
			return perspective( radians( fov ), float( screenWidth ) / float( screenHeight ), near, far );
		}

		case PROJECTION::ORTHOGRAPHIC:
		{
			const float halfHeight = screenHeight * 0.5f;
			const float halfWidth  = screenWidth * 0.5f;
			return ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, near, far );
		}

		default:
		{
			assert( false );
			return MAT4F_ID;
		}
		}
	}

} // namespace VTX::App::Scene
