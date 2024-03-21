#ifndef __VTX_UI_HELPER_ANIMATION__
#define __VTX_UI_HELPER_ANIMATION__

#include <util/math.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::UI::Helper::Animation
{
	Vec3f computeCameraOrientPosition(
		const Vec3f				 p_forward,
		const float				 p_fov,
		const Util::Math::AABB & p_target,
		const float				 p_orientZoomFactor = 0.666f
	);
}

#endif
