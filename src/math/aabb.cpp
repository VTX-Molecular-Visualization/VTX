#include "aabb.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Math
	{
		AABB::AABB( const Vec3f & p_min, const Vec3f & p_max ) : _min( p_min ), _max( p_max ) {}
		AABB::AABB( const Vec3f & p_center, const float p_radius ) :
			_min( p_center - p_radius ), _max( p_center - p_radius )
		{
		}

		void AABB::extend( const Vec3f & p_point )
		{
			_min = Util::Math::min( p_point, _min );
			_max = Util::Math::max( p_point, _max );
		}

		void AABB::extend( const AABB & p_aabb )
		{
			_min = Util::Math::min( _min, p_aabb._min );
			_max = Util::Math::max( _max, p_aabb._max );
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

		bool AABB::intersect( const Vec3f & p_rayPosition,
							  const Vec3f & p_rayInvDir,
							  const Vec3i & p_isDirNeg,
							  const float	p_tMin,
							  const float	p_tMax ) const
		{
			float t_min = ( _limits[ p_isDirNeg.x ].x - p_rayPosition.x ) * p_rayInvDir.x;
			float t_max = ( _limits[ 1 - p_isDirNeg.x ].x - p_rayPosition.x ) * p_rayInvDir.x;

			const float t_ymin = ( _limits[ p_isDirNeg.y ].y - p_rayPosition.y ) * p_rayInvDir.y;
			const float t_ymax = ( _limits[ 1 - p_isDirNeg.y ].y - p_rayPosition.y ) * p_rayInvDir.y;

			if ( t_min > t_ymax || t_ymin > t_max ) return false;
			if ( t_ymin > t_min ) t_min = t_ymin;
			if ( t_ymax < t_max ) t_max = t_ymax;

			const float t_zmin = ( _limits[ p_isDirNeg.z ].z - p_rayPosition.z ) * p_rayInvDir.z;
			const float t_zmax = ( _limits[ 1 - p_isDirNeg.z ].z - p_rayPosition.z ) * p_rayInvDir.z;

			if ( t_min > t_zmax || t_zmin > t_max ) return false;
			if ( t_zmin > t_min ) t_min = t_zmin;
			if ( t_zmax < t_max ) t_max = t_zmax;

			return ( ( t_min < p_tMax ) && ( t_max > p_tMin ) );
		}

		AABB AABB::join( const AABB & p_aabb1, const AABB & p_aabb2 )
		{
			return AABB( Util::Math::min( p_aabb1._min, p_aabb2._min ), Util::Math::max( p_aabb1._max, p_aabb2._max ) );
		}
	} // namespace Math
} // namespace VTX
