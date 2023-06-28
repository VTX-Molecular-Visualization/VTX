#include "util/math/aabb.hpp"
#include "util/math.hpp"

namespace VTX::Util::Math
{
	AABB::AABB( const Vec3f & p_min, const Vec3f & p_max ) : data { { p_min, p_max } } {}
	AABB::AABB( const Vec3f & p_center, const float p_radius ) : data { { p_center - p_radius, p_center + p_radius } }
	{
	}

	void AABB::extend( const Vec3f & p_point )
	{
		data.minMax.min = Util::Math::min( p_point, data.minMax.min );
		data.minMax.max = Util::Math::max( p_point, data.minMax.max );
	}

	void AABB::extend( const AABB & p_aabb )
	{
		data.minMax.min = Util::Math::min( data.minMax.min, p_aabb.data.minMax.min );
		data.minMax.max = Util::Math::max( data.minMax.max, p_aabb.data.minMax.max );
	}

	void AABB::extend( const Vec3f & p_center, const float p_radius )
	{
		extend( p_center - p_radius );
		extend( p_center + p_radius );
	}

	void AABB::translate( const Vec3f & p_translation )
	{
		data.minMax.min += p_translation;
		data.minMax.max += p_translation;
	}

	bool AABB::intersect( const Vec3f & p_rayPosition,
						  const Vec3f & p_rayInvDir,
						  const Vec3i & p_isDirNeg,
						  const float	p_tMin,
						  const float	p_tMax ) const
	{
		float tMin = ( data.limits[ p_isDirNeg.x ].x - p_rayPosition.x ) * p_rayInvDir.x;
		float tMax = ( data.limits[ 1 - p_isDirNeg.x ].x - p_rayPosition.x ) * p_rayInvDir.x;

		const float tYMin = ( data.limits[ p_isDirNeg.y ].y - p_rayPosition.y ) * p_rayInvDir.y;
		const float tYMax = ( data.limits[ 1 - p_isDirNeg.y ].y - p_rayPosition.y ) * p_rayInvDir.y;

		if ( tMin > tYMax || tYMin > tMax )
			return false;
		if ( tYMin > tMin )
			tMin = tYMin;
		if ( tYMax < tMax )
			tMax = tYMax;

		const float tZmin = ( data.limits[ p_isDirNeg.z ].z - p_rayPosition.z ) * p_rayInvDir.z;
		const float tZmax = ( data.limits[ 1 - p_isDirNeg.z ].z - p_rayPosition.z ) * p_rayInvDir.z;

		if ( tMin > tZmax || tZmin > tMax )
			return false;
		if ( tZmin > tMin )
			tMin = tZmin;
		if ( tZmax < tMax )
			tMax = tZmax;

		return ( ( tMin < p_tMax ) && ( tMax > p_tMin ) );
	}

	AABB AABB::join( const AABB & p_aabb1, const AABB & p_aabb2 )
	{
		return AABB( Util::Math::min( p_aabb1.data.minMax.min, p_aabb2.data.minMax.min ),
					 Util::Math::max( p_aabb1.data.minMax.max, p_aabb2.data.minMax.max ) );
	}
} // namespace VTX::Util::Math
