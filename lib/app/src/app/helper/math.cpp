#include "app/helper/math.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Helper::Math
{
	Util::Math::AABB applyTransformOnAABB( const Util::Math::AABB & p_aabb, const Util::Math::Transform & p_transform )
	{
		Util::Math::AABB res = Util::Math::AABB();

		std::vector<Vec3f> aabbSummits = p_aabb.getSummits();

		for ( const Vec3f & summit : aabbSummits )
		{
			const Vec4f worldSummit = p_transform.get() * Vec4f( summit, 1 );
			res.extend( worldSummit );
		}

		return res;
	}

} // namespace VTX::App::Helper::Math
