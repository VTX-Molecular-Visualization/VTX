#include "app/core/animation//helper.hpp"

namespace VTX::App::Core::Animation::Helper
{
	Vec3f computeCameraOrientPosition(
		const Vec3f				 p_forward,
		const float				 p_fov,
		const Util::Math::AABB & p_target,
		const float				 p_zoomFactor
	)
	{
		const float orientTargetDistance = p_target.radius() / std::tan( Util::Math::radians( p_fov ) * p_zoomFactor );
		return p_target.centroid() - ( p_forward * orientTargetDistance );
	}
} // namespace VTX::App::Core::Animation::Helper
