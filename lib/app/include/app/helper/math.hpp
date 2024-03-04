#ifndef __VTX_APP_HELPER_MATH__
#define __VTX_APP_HELPER_MATH__

#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Helper::Math
{
	Util::Math::AABB applyTransformOnAABB( const Util::Math::AABB & p_aabb, const Util::Math::Transform & p_transform );

} // namespace VTX::App::Helper::Math

#endif
