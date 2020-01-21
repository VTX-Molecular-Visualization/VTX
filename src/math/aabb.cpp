#include "aabb.hpp"
#include "../util/logger.hpp"
#include "../util/math.hpp"
#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>

namespace VTX
{
	namespace Math
	{
		AABB::AABB( const Vec3f & p_center, const float p_radius ) :
			_min( p_center - p_radius ), _max( p_center - p_radius )
		{
		}

		void AABB::extend( const Vec3f & p_point )
		{
			_min = Util::Math::min( p_point, _min );
			_max = Util::Math::max( p_point, _max );
		}

		void AABB::extend( const Vec3f & p_center, const float p_radius )
		{
			extend( p_center - p_radius );
			extend( p_center + p_radius );
		}

		void AABB::translate( const Vec3f & p_translation )
		{
			_min += p_translation;
			_max += p_translation;
		}

	} // namespace Math
} // namespace VTX
