#ifndef __VTX_APP_INTERNAL_CONSTANTS__
#define __VTX_APP_INTERNAL_CONSTANTS__

#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Internal
{
	// Constants.
	constexpr Vec3f CAMERA_RIGHT_DEFAULT = VEC3F_X;
	constexpr Vec3f CAMERA_UP_DEFAULT	 = VEC3F_Y;
	constexpr Vec3f CAMERA_FRONT_DEFAULT = Vec3f( 0.f, 0.f, -1.f );

	constexpr Vec3f RIGHT_AXIS = VEC3F_X;
	constexpr Vec3f UP_AXIS	   = VEC3F_Y;
	constexpr Vec3f FRONT_AXIS = Vec3f( 0.f, 0.f, -1.f );
} // namespace VTX::App::Internal

#endif
