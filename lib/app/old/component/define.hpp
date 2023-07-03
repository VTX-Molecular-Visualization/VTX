#ifndef __VTX_APP_OLD_COMPONENT_DEFINE__
#define __VTX_APP_OLD_COMPONENT_DEFINE__

#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Component
{
	// Constants.
	constexpr Vec3f CAMERA_RIGHT_DEFAULT = VEC3F_X;
	constexpr Vec3f CAMERA_UP_DEFAULT	 = VEC3F_Y;
	constexpr Vec3f CAMERA_FRONT_DEFAULT = Vec3f( 0.f, 0.f, -1.f );
} // namespace VTX::App::Old::Component

#endif
