#include "util/math/aabb.hpp"
#include "util/math.hpp"

namespace VTX::Util::Math
{
	AABB::AABB( const Vec3f & p_point ) : _min( p_point ), _max( p_point ) {}

	AABB::AABB( const Vec3f & p_min, const Vec3f & p_max ) : _min( p_min ), _max( p_max ) {}

	AABB::AABB( const Vec3f & p_center, const float p_radius ) :
		_min( p_center - p_radius ), _max( p_center + p_radius )
	{
	}

	void AABB::invalidate()
	{
		_min = VEC3F_MAX;
		_max = VEC3F_LOWEST;
	}

	std::vector<Vec3f> AABB::getSummits() const
	{
		return { { _min.x, _min.y, _min.z }, { _min.x, _min.y, _max.z }, { _min.x, _max.y, _min.z },
				 { _min.x, _max.y, _max.z }, { _max.x, _min.y, _min.z }, { _max.x, _min.y, _max.z },
				 { _max.x, _max.y, _min.z }, { _max.x, _max.y, _max.z } };
	}

	uint AABB::largestAxis() const
	{
		const Vec3f d = diagonal();
		if ( d.x > d.y && d.x > d.z )
		{
			return 0;
		}
		else if ( d.y > d.z )
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	float AABB::area() const
	{
		const Vec3f d = diagonal();
		return isValid() ? ( d.x * d.y + d.y * d.z + d.z * d.x ) * 2.f : 0.f;
	}

	Vec3f AABB::offset( const Vec3f & p_pt ) const
	{
		Vec3f o = p_pt - _min;
		if ( _max.x > _min.x )
		{
			o.x /= _max.x - _min.x;
		}
		if ( _max.y > _min.y )
		{
			o.y /= _max.y - _min.y;
		}
		if ( _max.z > _min.z )
		{
			o.z /= _max.z - _min.z;
		}
		return o;
	}

	float AABB::offset( const Vec3f & p_pt, const uint p_dim ) const
	{
		float o = p_pt[ p_dim ] - _min[ p_dim ];
		if ( _max[ p_dim ] > _min[ p_dim ] )
		{
			o /= _max[ p_dim ] - _min[ p_dim ];
		}
		return o;
	}

	/*
	bool AABB::intersect(
		const Vec3f & p_rayPosition,
		const Vec3f & p_rayInvDir,
		const Vec3i & p_isDirNeg,
		const float	  p_tMin,
		const float	  p_tMax
	) const
	{
		float tMin = ( data.limits[ p_isDirNeg.x ].x - p_rayPosition.x ) * p_rayInvDir.x;
		float tMax = ( data.limits[ 1 - p_isDirNeg.x ].x - p_rayPosition.x ) * p_rayInvDir.x;

		const float tYMin = ( data.limits[ p_isDirNeg.y ].y - p_rayPosition.y ) * p_rayInvDir.y;
		const float tYMax = ( data.limits[ 1 - p_isDirNeg.y ].y - p_rayPosition.y ) * p_rayInvDir.y;

		if ( tMin > tYMax || tYMin > tMax )
		{
			return false;
		}
		if ( tYMin > tMin )
		{
			tMin = tYMin;
		}
		if ( tYMax < tMax )
		{
			tMax = tYMax;
		}

		const float tZmin = ( data.limits[ p_isDirNeg.z ].z - p_rayPosition.z ) * p_rayInvDir.z;
		const float tZmax = ( data.limits[ 1 - p_isDirNeg.z ].z - p_rayPosition.z ) * p_rayInvDir.z;

		if ( tMin > tZmax || tZmin > tMax )
		{
			return false;
		}
		if ( tZmin > tMin )
		{
			tMin = tZmin;
		}
		if ( tZmax < tMax )
		{
			tMax = tZmax;
		}

		return ( ( tMin < p_tMax ) && ( tMax > p_tMin ) );
	}
	*/

	AABB AABB::join( const AABB & p_aabb1, const AABB & p_aabb2 )
	{
		return AABB( Util::Math::min( p_aabb1._min, p_aabb2._min ), Util::Math::max( p_aabb1._max, p_aabb2._max ) );
	}
} // namespace VTX::Util::Math
