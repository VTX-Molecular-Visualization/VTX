#include "ui/helper/animation.hpp"

namespace VTX::UI::Helper::Animation
{
	Vec3f computeCameraOrientPosition(
		const Vec3f				 p_forward,
		const float				 p_fov,
		const Util::Math::AABB & p_target,
		const float				 p_zoomFactor
	)
	{
		const float orientTargetDistance = p_target.radius() / std::tanf( Util::Math::radians( p_fov ) * p_zoomFactor );
		return p_target.centroid() - ( p_forward * orientTargetDistance );
	}
} // namespace VTX::UI::Helper::Animation
