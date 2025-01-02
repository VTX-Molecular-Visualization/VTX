#ifndef __VTX_UTIL_MATH_AABB__
#define __VTX_UTIL_MATH_AABB__

#include "util/constants.hpp"
#include "util/math.hpp"
#include "util/types.hpp"

namespace VTX::Util::Math
{
	class AABB
	{
	  public:
		AABB()	= default;
		~AABB() = default;

		AABB( const Vec3f & p_point );
		AABB( const Vec3f & p_min, const Vec3f & p_max );
		AABB( const Vec3f & p_center, const float p_radius );

		void invalidate()
		{
			_min = VEC3F_MAX;
			_max = VEC3F_LOWEST;
		}

		bool isValid() const { return ( ( _min.x <= _max.x ) && ( _min.y <= _max.y ) && ( _min.z <= _max.z ) ); }

		inline const Vec3f & getMin() const { return _min; }
		inline const Vec3f & getMax() const { return _max; }

		inline std::vector<Vec3f> getSummits() const
		{
			return { { _min.x, _min.y, _min.z }, { _min.x, _min.y, _max.z }, { _min.x, _max.y, _min.z },
					 { _min.x, _max.y, _max.z }, { _max.x, _min.y, _min.z }, { _max.x, _min.y, _max.z },
					 { _max.x, _max.y, _min.z }, { _max.x, _max.y, _max.z } };
		}

		void extend( const Vec3f & p_point );
		void extend( const AABB & p_aabb );
		void extend( const Vec3f & p_center, const float p_radius );

		void translate( const Vec3f & );

		Vec3f diagonal() const { return _max - _min; }
		float diameter() const { return isValid() ? Util::Math::length( diagonal() ) : 0.f; }
		float radius() const { return isValid() ? diameter() * 0.5f : 0.f; }
		Vec3f centroid() const { return ( _min + _max ) * 0.5f; }
		uint  largestAxis() const
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
		float area() const
		{
			const Vec3f d = diagonal();
			return isValid() ? ( d.x * d.y + d.y * d.z + d.z * d.x ) * 2.f : 0.f;
		}

		Vec3f offset( const Vec3f & p_pt ) const
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

		float offset( const Vec3f & p_pt, const uint p_dim ) const
		{
			float o = p_pt[ p_dim ] - _min[ p_dim ];
			if ( _max[ p_dim ] > _min[ p_dim ] )
			{
				o /= _max[ p_dim ] - _min[ p_dim ];
			}
			return o;
		}

		/*
		bool intersect(
			const Vec3f & p_rayPosition,
			const Vec3f & p_rayInvDir,
			const Vec3i & p_isDirNeg,
			const float	  p_tMin,
			const float	  p_tMax
		) const;
		*/

		static AABB join( const AABB & p_aabb1, const AABB & p_aabb2 );

	  private:
		Vec3f _min = VEC3F_MAX;
		Vec3f _max = VEC3F_LOWEST;
	};
} // namespace VTX::Util::Math

#endif
