#ifndef __VTX_APP_CORE_ANIMATION_HELPER__
#define __VTX_APP_CORE_ANIMATION_HELPER__

#include <util/math.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Animation::Helper
{
	Vec3f computeCameraOrientPosition(
		const Vec3f				 p_forward,
		const float				 p_fov,
		const Util::Math::AABB & p_target,
		const float				 p_orientZoomFactor = 0.666f
	);
}

#endif
